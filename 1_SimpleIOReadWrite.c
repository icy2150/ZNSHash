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
//기본적인 읽기 쓰기 Example 입니다.
int main(int argc, char **argv){
  struct xnvme_opts opts = xnvme_opts_default(); // xNVMe options  =  default 함수 있음
  struct xnvme_dev *dev;
  uint32_t nsid = 0x1; //Namespace identifier
  int err;

  opts.nsid = nsid; // 용도는?

//***********************************************************************************ㅊ
  // 디바이스 연결
  dev = xnvme_dev_open("0000:d8:00.0", &opts); //URL 의 주소에 option에 맞는 handle 생성
  // SPDK 안거치고 직접 조작하려면 아래 명령어 사용
  //dev = xnvme_dev_open("/dev/nvme0n1", &opts);
  if (!dev) { // ERR 출력
    perror("xnvme_dev_open()");
    return 1;
  }
//***********************************************************************************

//***********************************************************************************
  //OPEN으로 상태 변경
  struct xnvme_spec_znd_descr zone = {0}, after = {0};
  {
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_znd_mgmt_send(&ctx, nsid, zone.zslba, false, XNVME_SPEC_ZND_CMD_MGMT_SEND_OPEN, false, NULL);
    if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
      printf("xnvme_cmd_zone_mgmt(EOPEN)\n");
      xnvmec_perr("xnvmec_cli_to_opts()", err);
      return 0;
    }
  }
//***********************************************************************************



 
//***********************************************************************************
  //버퍼 할당
  void *dbuf = NULL;
  size_t dbuf_nbytes = 4096;
  dbuf = xnvme_buf_alloc(dev, dbuf_nbytes);
  if (!dbuf) {
    printf("xnvme_buf_alloc\n");
    return 0;
  }

  //파일 내용을 dbuf로
  err = xnvmec_buf_fill(dbuf, dbuf_nbytes, "anum");
  if (err) {
    printf("xnvmec_buf_fill\n");
    return 0;
  }
//***********************************************************************************

//***********************************************************************************
//WRITE

  int i;
  for(i=0; i<1; i++)
  {
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_nvm_write(&ctx, nsid, zone.zslba, 0, dbuf, NULL);
    if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
      printf("xnvme_nvm_write\n");
    }
    //printf("%d\n",i);
  }
/*
  //APPEND
{
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_znd_append(&ctx, nsid, zone.zslba, 0, dbuf, NULL);
    if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
      printf("xnvme_nvm_write!! %d \n", err);
    }
  }
*/



//***********************************************************************************


//***********************************************************************************
//READ
  void *rbuf = NULL;
  rbuf = xnvme_buf_alloc(dev, dbuf_nbytes);
  if (!rbuf) {
    printf("xnvme_buf_alloc\n");
    return 0;
  }

  {

    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    ctx = xnvme_cmd_ctx_from_dev(dev);
        err = xnvme_nvm_read(&ctx, nsid, zone.zslba, 0, rbuf, NULL);
    if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
      printf("xnvme_nvm_read\n");
    } else {
      printf("rbuf : %s\n", (char *)rbuf);
    }
    //printf("Size of rbuf : %ld\n",sizeof(rbuf));
  }

//***********************************************************************************


//***********************************************************************************
  //RESET
  {
    struct xnvme_cmd_ctx ctx = xnvme_cmd_ctx_from_dev(dev);
    err = xnvme_znd_mgmt_send(&ctx, nsid, zone.zslba, false, XNVME_SPEC_ZND_CMD_MGMT_SEND_RESET, false, NULL);
    if(err || xnvme_cmd_ctx_cpl_status(&ctx)) { printf("xnvme_cmd_zone_mgmt(EOPEN)\n"); return -err; }
  }
//***********************************************************************************
}

