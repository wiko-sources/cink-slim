#include <stdio.h>
#include <string.h>

#include <build_info.h>
#include <lib_sign_export.h>
#include <type_defs.h>


/**************************************************************************
 *  DEFINITIONS
 **************************************************************************/
#define MOD                         "SignTool"

/**************************************************************************
 *  MAIN FLOW
 **************************************************************************/
int main(int argc, char *argv[])
{          
    
    FILE *gen_fd;        
    char gen_header = FALSE;

    /* ------------------------------------- */
    /* dump information                      */
    /* ------------------------------------- */    
    MSG("\n=========================================\n");
    MSG("[Android SignTool]\n\n");
    MSG("Built at %s\n",BUILD_TIME);    
    MSG("=========================================\n\n");    

    /* ------------------------------------- */
    /* parse arguments                       */
    /* ------------------------------------- */    
    if (argc == 6)
    {
        MSG("[%s] sign image ... \n",MOD);
    }
    else if (argc == 5)
    {   
        MSG("[%s] generate hdr file ...\n",MOD);
        
        gen_header = TRUE;        
    }
    else
    {
        MSG("Usage:    Sign Image .. \n");
        MSG("          ./SignTool [KEY] [CONFIG] [INPUT_IMAGE] [OUTPUT_SIGNATURE] [OUTPUT_HEADER]\n\n");
        
        MSG("Example:\n");
        MSG("          ./SignTool IMG_KEY.ini IMG_CFG.ini u-boot.bin u-boot-signature u-boot-header\n\n");
                
        MSG("Usage:    Output Key Information for Linking .. \n");
        MSG("          ./SignTool [KEY] [CONFIG] [OUTPUT_C_HEADER] [OUTPUT_PREFIX]\n\n");
        
        MSG("Example:\n");
        MSG("          ./SignTool IMG_KEY.ini IMG_CFG.ini GEN_IMG_KEY.h IMG\n"  );
        return -1;

    }

    /* ------------------------------------- */
    /* open auto-gen header                  */
    /* ------------------------------------- */    
    if (TRUE == gen_header)
    {
        gen_fd = fopen(argv[3],"wb");
        fwrite ("// [",1,strlen("// ["),gen_fd);      
        fwrite (argv[3],1,strlen(argv[3]),gen_fd);              
        fwrite ("]\n// BUILD TIME : ",1,strlen("]\n// BUILD TIME : "),gen_fd);          
        fwrite (BUILD_TIME,1,strlen(BUILD_TIME),gen_fd);
        fwrite ("\n",1,strlen("\n"),gen_fd);   
    }

    /* ------------------------------------- */
    /* import key                            */
    /* ------------------------------------- */    
    if( 0 != imp_key(argv[1],argv[4],gen_header,gen_fd))
    {        
        return -1;
    }


    if (FALSE == gen_header)
    {

        /* ------------------------------------- */    
        /* check if this image is signed already */
        /* ------------------------------------- */
        if( 0 != chk_img(argv[3]))
        {        
            return -1;
        }

        /* ------------------------------------- */
        /* create and write header               */
        /* ------------------------------------- */        
        if( 0 != gen_hdr(argv[2],argv[5],argv[3],argv[4]))
        {        
            return -1;
        }

        /* ------------------------------------- */
        /* hash and sign image                   */
        /* ------------------------------------- */        
        if( 0 != pro_img(argv[4],argv[3],argv[5]))
        {        
            return -1;
        }
    }

    MSG("\n");

    return 0;
}

