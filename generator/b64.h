unsigned char * base64=(unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
 
CString base64encode(CString src,int srclen)
{
     int n,buflen,i,j;
     static unsigned char *dst;
     CString buf=src;
     buflen=n=srclen;
     dst=(unsigned char*)malloc(buflen/3*4+3);
     memset(dst,0,buflen/3*4+3);
     for(i=0,j=0;i<=buflen-3;i+=3,j+=4) {
         dst[j]=(buf[i]&0xFC)>>2;
         dst[j+1]=((buf[i]&0x03)<<4) + ((buf[i+1]&0xF0)>>4);
         dst[j+2]=((buf[i+1]&0x0F)<<2) + ((buf[i+2]&0xC0)>>6);
         dst[j+3]=buf[i+2]&0x3F;
     }
     if(n%3==1) {
          dst[j]=(buf[i]&0xFC)>>2;
         dst[j+1]=((buf[i]&0x03)<<4);
         dst[j+2]=64;
         dst[j+3]=64;
         j+=4;
     }
     else if(n%3==2) {
         dst[j]=(buf[i]&0xFC)>>2;
         dst[j+1]=((buf[i]&0x03)<<4)+((buf[i+1]&0xF0)>>4);
         dst[j+2]=((buf[i+1]&0x0F)<<2);
         dst[j+3]=64;
         j+=4;
     }
     for(i=0;i<j;i++) /* map 6 bit value to base64 ASCII character */
         dst[i]=base64[(int)dst[i]];
     dst[j]=0;
     return CString(dst);
}
