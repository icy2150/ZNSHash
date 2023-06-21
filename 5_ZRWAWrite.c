#include <stdio.h>
#include <time.h>
#include <libxnvme.h>
#include <libxnvme_pp.h>
#include <libxnvme_znd.h>
#include <libxnvme_nvm.h>
#include <libxnvme_spec.h>
#include <libxnvme_lba.h>
#include <libxnvmec.h>
#include <libxnvme_util.h>

#define LOC_LBA 8

//***************************************************
// ZRWA를 사용하는 코드입니다. 
// 0번 zone을 ZRWA True로 Open 하며,
// block을 입력하면 block에 write가 가능한지 알려줍니다.
//***************************************************

int main(int argc, char **argv)
{
  struct xnvme_opts opts = xnvme_opts_default();
  struct xnvme_dev *dev;
  uint32_t nsid = 0x1;
  struct xnvme_spec_znd_descr zone = {0}, after = {0};
  void *dbuf = NULL;
  void *dbuf2 = NULL;
  void *rbuf = NULL;
  size_t dbuf_nbytes = 4096;
  int err;
  clock_t start, end;

  opts.nsid = nsid; 

//dev
dev = xnvme_dev_open("0000:d8:00.0", &opts);
//dev = xnvme_dev_open("/dev/nvme0n1", &opts);
  if (!dev) {
    perror("xnvme_dev_open()");
    return 1;
  }
//  zone.zslba = 0;
  
  //printf("%d\n", xnvme_dev_pr(dev, XNVME_PR_DEF));  

  //ZRWA True 로 open (0번 zone)
  {
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_znd_mgmt_send(&ctx, nsid, zone.zslba + 0 * 24576, false, XNVME_SPEC_ZND_CMD_MGMT_SEND_OPEN, true, NULL);
    if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
      printf("xnvme_cmd_zone_mgmt(EOPEN)\n");
      xnvmec_perr("xnvmec_cli_to_opts()", err);	
      return 0;
    }
  }
  //printf("%d\n", xnvme_dev_pr(dev, XNVME_PR_DEF));



  dbuf = xnvme_buf_alloc(dev, dbuf_nbytes);
  if (!dbuf) {
    printf("xnvme_buf_alloc\n");
    return 0;
  }
  err = xnvmec_buf_fill(dbuf, dbuf_nbytes, "anum");
  if (err) {
    printf("xnvmec_buf_fill\n");
    return 0;
  }

  int testidx;
  int temp = 0;
  while(1){
    puts("Enter LBA NUMBER (-1 to end) : ");
    scanf("%d",&testidx);
    if(testidx == -1)
      break;
    
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_nvm_write(&ctx, nsid, testidx, 0, dbuf + temp, NULL);
    temp = temp + 1;
    if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
      printf("ERROR!!(zrwa window is 32) %d \n", err);
      break;
    }
    else{
      printf("success \n");
    }
  }

/*
  {
    puts("Reset in progress\n");
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_znd_mgmt_send(&ctx, nsid, zone.zslba, false, XNVME_SPEC_ZND_CMD_MGMT_SEND_RESET, false, NULL);
    if(err || xnvme_cmd_ctx_cpl_status(&ctx)) { printf("xnvme_cmd_zone_mgmt(EOPEN)\n"); return -err; }
  }
*/

  xnvme_dev_close(dev);

  return 0;
}
