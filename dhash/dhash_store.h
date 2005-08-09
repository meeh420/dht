#ifndef __DHASH_STORE__H_
#define __DHASH_STORE__H_

#include <dhash_prot.h>
#include <dhash_common.h>
#include <dhash.h>
#include <route.h>
#include <chord.h>

typedef callback<void, dhash_stat, chordID, bool>::ref cbclistore_t;

//store a block/fragment by sending RPCs smaller than the MTU
class dhash_store  {

public:
  
  static void execute (ptr<vnode> clntnode, ptr<location> dest, 
		       blockID bid,
		       str data, 
		       cbclistore_t cb, 
		       store_status store_type = DHASH_STORE)
  {
    vNew dhash_store (clntnode, dest, bid, data, store_type, cb);
  }


protected:
  uint npending;
  bool error;
  dhash_stat status;

  ptr<location> dest;
  str data;
  blockID bid;
  cbclistore_t cb;
  dhash_ctype ctype;
  store_status store_type;
  ptr<vnode> clntnode;

  int num_retries;
  timecb_t *dcb;
  bool present;

  ptr<bool> deleted;

  dhash_store (ptr<vnode> clntnode, ptr<location> dest, blockID bid,
               str _block, store_status store_type, 
	       cbclistore_t cb)
    : npending (0),
      error (false),
      status (DHASH_OK),
      dest (dest), data (_block), bid (bid), cb (cb),
      ctype (bid.ctype), store_type (store_type),
      clntnode (clntnode), num_retries (0),
      dcb (NULL),
      present (false),
      deleted (New refcounted<bool> (false))
  {
    start ();
  }
  

  ~dhash_store ()
  {
    if (dcb) timecb_remove (dcb);
  }

  void done (bool present);
  void timed_out (ptr<bool> deleted);
  void start ();
  void finish (ptr<bool> deleted, ptr<dhash_storeres> res, int num, clnt_stat err);
  void store (ptr<location> dest, blockID blockID, char *data, size_t len,
	      size_t off, size_t totsz, int num, dhash_ctype ctype, 
	      store_status store_type);  

};

#endif
