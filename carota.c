#include <assert.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <linux/types.h>
#include <limits.h> //PATH_MAX from here
#include <eva.h>
#include <deviceInfo.h>

#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

char PackageFileName[256];

unsigned char updImgName[1024] = { 0 };
unsigned char updImgVersion[1024] = { 0 };
unsigned char packTarName[1024] = { 0 };
char template[] = "/tmp/tmpXXXXXX";
char * packFolder;
int compoentFlag = 0, keyFlag = 0, AiFlag = 0;

void CreateKeyAndCertificate()
{
    struct certInfo certificateInfo; 
    //char countryName[3]={0},state[33]={0},localityName[65]={0};
    //char orgName[256]={0},orgUnitName[256]={0},commonName[65]={0},emailAddress[65]={0};
    int validityPeriod=365;

    int ret;

    char *countryName = "TW";
    char *state = "TAIWAN";
    char *localityName = "NEW TAIPEI CITY";
    char *orgName = "ADLINK";
    char *orgUnitName = "TPDC-SEC";
    char *commonName = "ADLINK";
    char *emailAddress = "nathanch.tu@adlinktech.com";

    certificateInfo.ValidityPeriod=validityPeriod;
    certificateInfo.CountryName=countryName;
    certificateInfo.State=state;
    certificateInfo.LocalityName=localityName;
    certificateInfo.OrganizationName=orgName;
    certificateInfo.OrganizationalUnitName=orgUnitName;
    certificateInfo.CommonName=commonName;
    certificateInfo.Email=emailAddress;
    char priKeyFileName[1024]={0},certFileName[1024]={0};


    char *priKeyFileName1 = "updPriv";
    //char *pubKeyFile = "updPub.key";
    char *siCertFileName1 = "updPub.crt"; //updPub.crt
    memset(priKeyFileName, '\0', sizeof(priKeyFileName));
    strcpy(priKeyFileName, "updPriv");

    memset(certFileName, '\0', sizeof(certFileName));
    strcpy(certFileName, "updPub.crt");



    printf("=========================================Create Key And Certificate==============================================\n");

    ret = createKeyPair(priKeyFileName);
    if(ret != 0)
        {printf(" createKeyPair error= %d\n", ret);}

    strcat(priKeyFileName, ".priv");
    

    ret = createCertificate(priKeyFileName,certFileName,&certificateInfo);
    if(ret != 0)
        {printf(" create Certificate error= %d\n", ret);}

}
void CreateModelImage()
{
    int ret;
    char *modelSchema = "";//"/EVA-EXAMPLE/CAROTA/ModelZoo.Schema"; //ModelZoo.Schema
    //char *modelFile = "yolo.bin";
    char *modeMetaDataFile1 = "aiModelMeta1.cfg";
    char *modelMetaDataFile2 = "aiModelMeta2.cfg";
    char *aiUpdImg = "aiModel_v1.1.zip";
    char *outModelUpdImage = "aiModelOutputUpdImage_v1.1.zip";
    char inModelFileName[1024] = { 0 };
    char outModelImageName[1024] = { 0 };
    char modelFile[1024] = { 0 };
    char input[64];
    char szDestination[1024] = {0};//"/EVA-Example/CAROTA/Model/";
    //char szDestination[1024] = "/EVA-Example/CAROTA/TensorModel/";
    

    printf("====================================Create Model Image==============================================\n");

    memset(inModelFileName, '\0', sizeof(inModelFileName));
    strcpy(inModelFileName, "ModelZoo.Schema");
    // Create AI model update image base
    ret = createModelUpdImg(inModelFileName,"./aiModel_v1.1.zip"); 
    if(ret !=0)
    {
            printf(RED_BOLD" createModelUpdImg err= %d\n"RESET, ret);
            return;
    }


    // Add model file into update image:
    printf(BLU_BOLD"Add model file into update image. Which model file do you want to add into image?\n"RESET);
    printf(BLU_BOLD"Select OpenVino or TensorRT (O/T)?\n"RESET);
    scanf("%s", input);
    printf("====================================================================================================\n");
    if(strcmp(input, "O") ==0 )
    {
        system("cd /Model");
        system("ls /Model | grep .bin");

        strcpy(szDestination, "/Model/");
    }
    else if(strcmp(input, "T") ==0 )
    {
        system("cd /TensorModel");
        system("ls /TensorModel | grep .engine");

        strcpy(szDestination, "/TensorModel/");
    }

    memset(input, '\0', sizeof(input));
    scanf("%s", input);

    strcat(szDestination, input);

    memset(modelFile, '\0', sizeof(modelFile));
    strcpy(modelFile, szDestination);  //"yolo.bin"
    ret = addFilesToModelUpdImg(modelFile, "aiModel_v1.1.zip");
    if(ret != 0)
        {printf(RED_BOLD" addFilesToModelUpdImg err= %d\n"RESET, ret);}

    // Check model update image integrity. Note: outModelUpdImage will be the final output update image
    ret = completeModelUpdImg(aiUpdImg, outModelUpdImage);
    if(ret != 0)
        {printf(RED_BOLD" completeModelUpdImg err= %d\n"RESET, ret);}

}

void CheckModelList(int signedFlag)
{
    int listCount=0,i;
    char pkgFileName[1024] = { 0 };
    char *imgFileList[10];
    char imgFiles[10][256] = {{0}};
    char input[64];
    char outputNoSighPackage[256];

    if(signedFlag == 0 )
    {
        strcat(PackageFileName, ".NoSigned.zip");
    }
    else if(signedFlag == 1 )
    {
        strcat(PackageFileName, ".Signed.zip");
    } 

    memset(pkgFileName, '\0', sizeof(pkgFileName)); //PackageFileName
    strcpy(pkgFileName, PackageFileName); // "UpdPkg_v1.1.NoSign.zip"

    printf("Package Name: %s\n",pkgFileName);


    for(i=0;i<10;i++){
        imgFileList[i] = (char*)&imgFiles[i];
    }
    listUpdImgInPkg(pkgFileName,imgFileList,&listCount);
    printf("Total Update Image: %d\n",listCount);
    printf("=================================Package of files are list as following===================== \n");
    for(i=0;i<listCount;i++){
        printf("%s\n",imgFileList[i]);
    }
    printf("=============================================================================================\n");
    printf(YEL_BOLD"(1) Please upload the package into the carota server. https://ota.sandbox.limios.net \n"RESET);
    printf(YEL_BOLD"(2) Please record the package id(PKGID) form the %s\n"RESET,pkgFileName);
    printf(YEL_BOLD"(3) Please assign the PKGID to target version.\n"RESET);
    printf("=============================================================================================\n");
}

const char* getFileName(const char* filename)
{
    const char* slash = strrchr(filename, '/');
    if (!slash || slash == filename)
        return filename;
    return slash + 1;
}

int remove_dir(const char* dir)
{
    char cur_dir[] = ".";
    char up_dir[] = "..";
    char dir_name[PATH_MAX+1];
    DIR* dirp;
    struct dirent* dp;
    struct stat dir_stat;

    //folder not exist
    if (0 != access(dir, F_OK)) {
        return 0;
    }

    //can not get attribute
    if (0 > stat(dir, &dir_stat)) {
        perror("get directory stat error");
        return -1;
    }

    if (S_ISREG(dir_stat.st_mode)) { //file
        remove(dir);
    } else if (S_ISDIR(dir_stat.st_mode)) { //is folder
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != NULL) {
            //ignore . & ..
            if ((0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name))) {
                continue;
            }

            sprintf(dir_name, "%s/%s", dir, dp->d_name);
            remove_dir(dir_name); //had sub folder
        }
        closedir(dirp);

        rmdir(dir); //erase empty folder
    } else {
        perror("unknow file type!");
    }

    return 0;
}


void CreateModelPackage()
{
    int ret;
    enum updComponent compoent;
    compoent = AI_MODEL_UPDATE;

    char *aipipeline = "EVAConfig.zip";
    char *outModelUpdImage = "aiModelOutputUpdImage_v1.1.zip"; //packageImage
    char *outNoSignUpdPkgName ;//= "UpdPkg_v1.1.NoSign.zip"; //no sign packageImage
    char *aiUpdPkg = "aiUpdatePkg.zip";
    char *installationPrivKey = "updPriv.priv";
    char *siCertFile = "updPub.crt";
    int verCheck = 0;
    int transactionFlag = 1;
    char *outputSignedFile = "signedAiUpdatePkg.zip";
    char src[256], dest[256], input[64];
    char src1[256], outNoSignUpdPkgName1[256];
    char command[512];
    char fileSrc[256], fileDest[256];
    int signFlag =0;
    
    
    printf("==================================Add Model Image into Package=====================================\n");

    // Add update image to update package
    //    if verCheck=1, will only update if version in schema file is > current version in asset
    ret = addUpdImgToPkg(aiUpdPkg ,outModelUpdImage ,compoent, verCheck); //output the aiUpdPkg

    if(ret != 0)
        {printf(RED_BOLD" Model Image package is fail. addUpdImgToPkg err= %d\n"RESET, ret); return;}
    else
        {printf(GRN_BOLD" Model Image package is ready. Please do next step. err= %d\n"RESET, ret);}

    printf("=================================Add EVA's config Image into Package===============================\n");
    //AI_PIPELINE_UPDATE
    printf("Compoent flag = %d\n", compoentFlag);
    if(compoentFlag == AI_PIPELINE_UPDATE)
    {
         compoent = AI_PIPELINE_UPDATE;

        ret = addUpdImgToPkg(aiUpdPkg ,aipipeline ,compoent, verCheck); //output the aiUpdPkg

        if(ret != 0)
            {printf(RED_BOLD" EVA config package are fail. addUpdImgToPkg Evaconfig err= %d\n"RESET, ret); return;}
        else
            {printf(GRN_BOLD" EVA config package are ready. Please do next step. err= %d\n"RESET, ret);}
    }

    printf("================================Creating EVA Model (Sign/Non-Sign)Package==========================\n");

    printf(BLU_BOLD"Would you like to sign the package?(Yes/No)\n"RESET);
    scanf("%s", input);

    if(strcmp(input, "Yes") ==0 )
    {
        //strcpy(src, ".Sign");
        signFlag = 1;
    }
    else if(strcmp(input, "No") == 0)
    {
        //strcpy(src, ".NoSign");
        signFlag = 0;
    } 

    strcpy(PackageFileName, "UpdPkg_v");     
    memset(input, 0 , sizeof(input)); 

    printf(BLU_BOLD"Creating the package's version (For example: 1.1, 1.2)...\n"RESET);
    scanf("%s", input);

    strcpy(dest, input);
    //strcat(dest, src);
    strcat(PackageFileName, dest);
    
    if(signFlag == 1){
        //int completeSignUpdPkg(char *pkgFile, char *privKey,char *certFile, char *signedPkgFile);
        ret = completeSignUpdPkg(aiUpdPkg, installationPrivKey,siCertFile, PackageFileName);
        if(ret==0){printf(GRN_BOLD"Signed package is sucessfully.\n"RESET);}
        else{printf(RED_BOLD"Signed package is fail\n"RESET);}
    }
    else{
        ret = completeNoSignUpdPkg(aiUpdPkg,PackageFileName);//UpdPkg_v1.0.NoSign.zip
        if(ret==0){printf(GRN_BOLD"Non-Signed package is sucessfully.\n"RESET);}
        else{printf(RED_BOLD"Non-Signed package is fail\n"RESET);}
    }
    
    CheckModelList(signFlag);

}

void deleteFile()
{
    char FileLocation[1024];
    char command[1024];

    //FileLocation[0][1024] ="/EVA-Example/CAROTA/aiModelOutputUpdImage_v1.1.zip";
    //FileLocation[1][1024] ="/EVA-Example/CAROTA/aiModel_v1.1.zip";
    //FileLocation[2][1024] ="/EVA-Example/CAROTA/aiUpdatePkg.zip";

    memset(FileLocation, 0 , sizeof(FileLocation));  
    strcpy(FileLocation, "/aiModelOutputUpdImage_v1.1.zip");
    sprintf(command, "rm -rf %s", FileLocation);
    system(command);

    memset(FileLocation, 0 , sizeof(FileLocation));  
    strcpy(FileLocation, "/aiModel_v1.1.zip");
    sprintf(command, "rm -rf %s", FileLocation);
    system(command);

    memset(FileLocation, 0 , sizeof(FileLocation));  
    strcpy(FileLocation, "/aiUpdatePkg.zip");
    sprintf(command, "rm -rf %s", FileLocation);
    system(command);
}

int CreateAIConfig(char* sampleMode)
{
    char command[64] ={ 0 };
    char aiConfig[1024] ={ 0 };
    char FileLocation[1024] = { 0 };

    printf("CreateAIConfig :%s\n", sampleMode); 

    memset(aiConfig, '\0', sizeof(aiConfig));
    memset(FileLocation, 0 , sizeof(FileLocation));  
    strcpy(FileLocation, "/AI.zip");

    if( access( FileLocation, F_OK ) == 0 ) {
         sprintf(command, "rm -rf %s", FileLocation);
         system(command);
         printf("AI file exist :%s\n", FileLocation);    // file doesn't exist
    } else {
        printf("AI file doesn't exist :%s\n", FileLocation);    // file doesn't exist
    }

    if(strcmp(sampleMode, "0") ==0 )
    { strcpy(aiConfig, "zip -r -j AI Classfication/*");}
    else if(strcmp(sampleMode, "1") ==0 )
    { strcpy(aiConfig, "zip -r -j AI Face/*");}
    else if(strcmp(sampleMode, "2") ==0 )
    { strcpy(aiConfig, "zip -r -j AI PCBA/*");}
    else if(strcmp(sampleMode, "3") ==0 )
    { strcpy(aiConfig, "zip -r -j AI Detection/*");}
    else if(strcmp(sampleMode, "4") ==0 )
    { strcpy(aiConfig, "zip -r -j AI MobileNet_SSD/*");}
    else if(strcmp(sampleMode, "5") ==0 )
    { strcpy(aiConfig, "zip -r -j AI Geofence/*");}
    else if(strcmp(sampleMode, "6") ==0 )
    { strcpy(aiConfig, "zip -r -j AI Weardetection/*");}
    else{ return -1;}

    system(aiConfig);

    return 0;
   
}

int CreatePipelineImagePack()
{
    int cmd_opt = 0, ret = 0;
    FILE *fp;
    unsigned char tmp[PATH_MAX+1] = { 0 };
    char * CurrentWD=NULL;
    char buf[255],md5Str[33]={0},devId[20]={0};
    char input[64];
    char command[64] ={ 0 };


    // Add pipeline file into update image:
    //printf("Add pipieline file into update image. would you like pack the file into image.(Yes/No)?\n");
    //scanf("%s", input);
    
    //if(strcmp(input, "Yes") ==0 )
    {
        //CurrentWD=getcwd(NULL, 0);

        printf("Path is :%s\n", CurrentWD);

        memset(updImgName, '\0', sizeof(updImgName));
        strcpy(updImgName, "/AI.zip");

        memset(packTarName, '\0', sizeof(packTarName));
        strcpy(packTarName, "/EVAConfig.zip");

        if (strlen(updImgName) == 0 || strlen(packTarName) == 0) {    
            printf("updImgName=%s,updImgVersion=%s,packTarName=%s\n",updImgName,updImgVersion,packTarName);
            return -1;
        }

        if( access( packTarName, F_OK ) == 0 ) {
            sprintf(command, "rm -rf %s", packTarName);
            system(command);
        } else {
            // file doesn't exist
        }
        packFolder = mkdtemp(template);
        sprintf(tmp, "md5sum %s", updImgName); 
        printf("Command:%s\n",tmp);

        fp = popen(tmp, "r");
        if (fp != NULL) {
            while (fgets(buf, sizeof(buf), fp) != NULL) {

                printf("%s",buf);
                if(strstr(buf, updImgName) != NULL){
                    memcpy(md5Str,buf,32);
                    printf("md5=%s\n",md5Str);
                }
            }
            pclose(fp);
        }
        sprintf(tmp,"%s/FileList",packFolder);
        fp = fopen(tmp,"w");
        if(fp==NULL){
            printf("Error: Can not create FileList!\n");
            return -1;
        }
        if(devId[0]==0)
            fprintf(fp,"Img=%s\nMD5=%s\nVersion=%s",getFileName(updImgName),md5Str,updImgVersion);
        else
            fprintf(fp,"Img=%s\nDEVID=%s\nMD5=%s\nVersion=%s",getFileName(updImgName),devId,md5Str,updImgVersion);
        fclose(fp);
        sprintf(tmp,"cp -f %s %s",updImgName,packFolder);
        ret = system(tmp);

        printf("system command is :%s\n", tmp);

        /*if(strlen(updImgName)==strlen(getFileName(updImgName)))
            sprintf(tmp,"tar czvf %s/%s *",CurrentWD,packTarName);
        else
            sprintf(tmp,"tar czvf %s *",packTarName);*/
        //zip file data/*
        //sprintf(tmp,"tar czvf %s *",packTarName);

        sprintf(tmp,"zip %s *",packTarName);

        ret = chdir(packFolder);
        ret = system(tmp);
        remove_dir(packFolder);
        system("exit");
        printf("Pack update image complete!%s\n", tmp);
        printf("Pack update image path:%s\n", packFolder);
        return ret;
    }

    system("exit");  
    return ret;
}

void help_usage(void)
{
    printf("Option: \n");
    printf("	-p [ Adding EVA AI configuration into package]\n");
    printf("	-m [ Packing the AI config=> 0:Classfication(GoogleNet), 1:Face Detection(ssd), 2:PCBA(ssd), 3:Detection(yolo)], 4:Classfication(ssd), 5:Geofence(ssd), 6:Weardetection(ssd)\n");
    printf("	-k [ Creating the new key and certificate.]\n");
    printf("	-h [ This is help message. ]\n");
    return;
}


int main(int argc, char** argv)
{
    int ret=0;
    char input[64];
    int cmd_opt = 0;
    char inferenceMode[1024] ={ 0 };

    while (1) {
        cmd_opt = getopt(argc, argv, "pkhm:");

        /* End condition always first */
        if (cmd_opt == -1) {
            break;
        }

        /* Lets parse */
        switch (cmd_opt) {
            case 'h':
            case '?':
                help_usage();
                return 0;
            case 'm':
                strcpy(inferenceMode, optarg);
                AiFlag = 1;
                break;
            case 'p':
                compoentFlag = 4;
                break;
            case 'k':
                keyFlag = 1;
                break;
        }
    }
    //printf(GRN_BOLD"This line will be print out with color green and font bold.\n"RESET);
    if(AiFlag == 1)
    {
        
        if(CreateAIConfig(inferenceMode)!=0)
        {
            printf("Pack the EVA AI configuration error!\n");      
        }
        else{ 
            printf("Pack the EVA AI configuration successfully.\n");
            // Add eva config file into update image:
            if(compoentFlag == 4)
            {
                printf(BLU_BOLD"Add EVA configuration file into update image. Would you like creat new file into image.(Yes/No)?\n"RESET);
                scanf("%s", input);

                if(strcmp(input, "Yes") ==0 )
                {
                    CreatePipelineImagePack();
                    return 0;
                }
                else if(strcmp(input, "No") == 0)
                {
                    printf("Doing the package model in the next step.\n");
                }
            }
        }
        return 0;
    }


    memset(input, '\0', sizeof(input));
         
    if(keyFlag == 1)
    {
        // Create the new key and Certificate
        printf(BLU_BOLD"Do you want to create the key and Certificate? (Yes/No)?\n"RESET);
        scanf("%s", input);

        if(strcmp(input, "Yes") ==0 )
        {
            CreateKeyAndCertificate();
            printf("Creating the public/private key pair and public Certificate.\n");
        }
        else if(strcmp(input, "No") == 0)
        {
            printf("Doing the package model in the next step.\n");
        }
    }

    //CreatePipelineImagePack();
    //sleep(3);
    deleteFile();

    CreateModelImage();
    sleep(3);
    CreateModelPackage();

    /*setProjToken("b9DDH1pEpT");
    printf("BRAND: %s\n",getDevBrand());
    printf("MODEL: %s\n",getDevModel());
    printf("UID: %s \n",getDevID());
    printf("PKG ID: %s\n",getPackageID());
    printf("TOKEN: %s\n",getProjToken());*/
    
    return ret;
}
