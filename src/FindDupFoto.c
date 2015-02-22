/*
 * FindDupFoto.c
 *
 *  Created on: Feb 19, 2015
 *      Author: LarchCoder
 */
#include <stdio.h>
#include <stdlib.h>

#include <apr-1/apr_general.h>
#include <apr-1/apr_file_info.h>
#include <apr-1/apr_file_io.h>
#include <apr-1/apr_md5.h>
#include "apr-1/apr_dbd.h"

#define PRINT_APR_ERROR(err) {\
								char errbuffer[100];\
								printf("APR ERROR:%s\n", apr_strerror(err,errbuffer,100));\
							 }


calc_file_md5sum(apr_pool_t *p, char *fullfilename)
{
	char buff[5];
	apr_file_t *myfile;
	apr_size_t size=5;
	apr_status_t ret;
	unsigned char digest[APR_MD5_DIGESTSIZE];
	char *md5;

	apr_md5_ctx_t md5_ctx;


	ret =apr_file_open(&myfile,fullfilename,APR_FOPEN_READ, APR_FPROT_UREAD,p);

	ret = apr_md5_init(&md5_ctx);

	while ((ret = apr_file_read(myfile,buff,&size)) == APR_SUCCESS)
	{
		apr_md5_update(&md5_ctx,buff,size);
	}
	apr_md5_final(digest,&md5_ctx);
	md5 = apr_psprintf(p, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
									digest[0],digest[1],digest[2],digest[3],digest[4],digest[5],digest[6],digest[7],digest[8],digest[9],
									digest[10],digest[11],digest[12],digest[13],digest[14],digest[15]);

	//ret = apr_file_read(myfile,buff,&size);	/* APR_EOF*/

	ret = apr_file_close(myfile);
	return md5;

}


insert_into_db(apr_pool_t *p, char *fulldir, char *dir, char *filename, char *md5sum)
{
	apr_status_t ret;
    apr_dbd_t* handle = NULL;
    const apr_dbd_driver_t* driver = NULL;
    int nrows;

    char *sql;

    ret = apr_dbd_init(p);
    ret = apr_dbd_get_driver(p, "sqlite3", &driver);
    ret = apr_dbd_open(driver, p, "photos.db", &handle);


    sql = apr_psprintf(p, "insert into photos (fulpath,dir,filename,md5) "
    						"values ('%s','%s','%s','%s');", fulldir,dir,filename,md5sum);

    ret = apr_dbd_query(driver, handle, &nrows, sql);

    ret = apr_dbd_close(driver, handle);

}

int find_file_in_dir(apr_pool_t *p, char *indir)
{
	apr_status_t ret;

	apr_dir_t *dir;
	apr_finfo_t file;
	apr_array_header_t *xx;

	char *subdir,*fullfilename;
	char *md5;

	char separator_string[2] = { '\0', '\0' };
	char *part, *ptr,*tmpDir,*FolderName;


    ret = apr_dir_open(&dir,indir,p);
    if (ret != 0)
    {
    	PRINT_APR_ERROR(ret);
    	return -1;
    }

    tmpDir = apr_pstrdup(p, indir);
	separator_string[0] = '/';
	while ((part = apr_strtok(tmpDir, separator_string, &ptr)) != NULL)
	{
        if (*part == '\0')      /* Ignore empty path components. */
            continue;
        tmpDir = NULL;		/* For the next call to apr_strtok */
        FolderName = part;
	}

    while ((ret = apr_dir_read(&file,APR_FINFO_MIN,dir)) != APR_ENOENT)
    {
    	if (ret != 0)
    		return -2;

    	if (file.filetype == APR_DIR)
    	{
    		if (apr_strnatcmp(file.name,".") == 0)
    			continue;
    		if (apr_strnatcmp(file.name,"..") == 0)
    		    continue;

    		subdir = apr_psprintf(p, "%s/%s", indir,file.name);
    		find_file_in_dir(p, subdir);
    	}
    	if (file.filetype == APR_REG)
    	{
    		apr_pool_t *p1;

    		apr_pool_create(&p1, p);
    		fullfilename = apr_psprintf(p1, "%s/%s", indir,file.name);
    		md5 = calc_file_md5sum(p1,fullfilename);
    		printf("%s----%s----%s---%s\n",indir,FolderName,file.name,md5);
    		md5 = apr_pstrdup(p,md5);

    		insert_into_db(p1,indir,FolderName,file.name,md5);

    		apr_pool_destroy(p1);

    	}
    }
}


static int create_db(apr_pool_t *p)
{

	apr_status_t ret;
    apr_dbd_t* handle = NULL;
    const apr_dbd_driver_t* driver = NULL;
    int nrows;
    apr_status_t rv;

    const char *sql = "create table IF NOT EXISTS photos (id INTEGER PRIMARY KEY,fulpath TEXT,dir TEXT,filename TEXT,md5 TEXT);";

    ret = apr_dbd_init(p);
    ret = apr_dbd_get_driver(p, "sqlite3", &driver);
    if (ret == APR_EDSOOPEN)
    {
    	printf("Compile apr-utils with sqlite \n",ret);
    }
    ret = apr_dbd_open(driver, p, "photos.db", &handle);

    rv = apr_dbd_query(driver, handle, &nrows, sql);

    ret = apr_dbd_close(driver, handle);

    return 1;
}


int main(int argc, const char * const * argv, const char * const *env)
{
	apr_status_t ret;
	apr_pool_t *p;

	apr_app_initialize(&argc, &argv, &env);

	apr_pool_create(&p, NULL);

	create_db(p);

	find_file_in_dir(p,"/tmp/backup_data/test/");




    apr_pool_destroy(p);
    apr_terminate();

    return EXIT_SUCCESS;
}


