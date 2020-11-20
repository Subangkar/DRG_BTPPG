#include <uib_util.h>

#include <curl/curl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <dlog.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>

#define CONFIG_SERVER_URL "http://192.168.1.104:8000/config.json" // "http://hr-logger.herokuapp.com/config/" //
#define CONFIG_FILE_NAME "config.json"
#define CURL_MAX_TRANS_TIME 60L
#define CURL_MAX_CONNECT_TIME 20L


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int downloadFile(const char *server_url, const char *filename, const char* filePath) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    long response_code=0;

    if (curl) {
        fp = fopen(filePath,"wb");
        if (!fp)
          return 1; /* can't continue */
        curl_easy_setopt(curl, CURLOPT_URL, server_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        dlog_print(DLOG_WARN, LOG_TAG, "curl_easy_perform() tried\n");
        /* Check for errors */
        if (res != CURLE_OK)
        {
          dlog_print(DLOG_ERROR, LOG_TAG, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
          return 1;
        }
        else
        {
          /* now extract transfer info */
          curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &response_code);
          dlog_print(DLOG_INFO, LOG_TAG, "response code: %d\n", response_code);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    return response_code==200 ? 0:1; // 0;
}

int download_config_file(const char* dir){
  char filePath[256];
  strcpy(filePath, dir);
  if(filePath[strlen(filePath)-1]!='/'){
    strcat(filePath, "/");
  }
  int pathSize = strlen(filePath);
  char* filename = CONFIG_FILE_NAME;
  strcpy(filePath+pathSize, filename);
  if(!downloadFile(CONFIG_SERVER_URL, filename, filePath))
  	  dlog_print(DLOG_INFO, LOG_TAG, "\"%s\" Downloaded\n", filename);
  else{
	  dlog_print(DLOG_ERROR, LOG_TAG, "\"%s\" could NOT be Downloaded\n", filename);
	  return 1;
  }
  return 0;
}
