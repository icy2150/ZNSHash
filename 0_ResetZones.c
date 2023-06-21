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
#include <string.h>

#define ZONE_EXP_START 0        // Start of Zone
#define ZONE_EXP_COUNT 40704      // Num of Zone to Execute

int main(int argc, char **argv){
  struct xnvme_opts opts = xnvme_opts_default(); // xNVMe options
  //xnvme_opts_pr(&opts,0);
  //printf("%s\n",opts.sync);
  struct xnvme_dev *dev;
  uint32_t nsid = 0x1;
  int err;
  

  opts.nsid = nsid;


//***********************************************************************************ㅊ
  // 디바이스 연결
  dev = xnvme_dev_open("0000:d8:00.0", &opts);
  // SPDK 안거치고 직접 조작하려면 아래 명령어 사용
  //dev = xnvme_dev_open("/dev/nvme0n1", &opts);
  if (!dev) { // ERR 출력
    perror("xnvme_dev_open()");
    return 1;
  }      
//RESET
  struct xnvme_spec_znd_descr zone = {0};
  for(int i=0; i<ZONE_EXP_COUNT;i++){
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_znd_mgmt_send(&ctx, nsid, zone.zslba+ (i + ZONE_EXP_START) * 24576, false, XNVME_SPEC_ZND_CMD_MGMT_SEND_RESET, false, NULL);
    if(err || xnvme_cmd_ctx_cpl_status(&ctx)) { printf("xnvme_cmd_zone_mgmt(EOPEN)\n"); return -err; }
  }
}
