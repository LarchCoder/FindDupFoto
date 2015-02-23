/*
 * find_missing.c
 *
 *  Created on: Jan 22, 2012
 *      Author: peterz
 */

#include <stdio.h>
#include <stdlib.h>

#include <apr-1/apr_general.h>
#include <apr-1/apr_file_info.h>
#include <apr-1/apr_file_io.h>
#include <apr-1/apr_md5.h>
#include "apr-1/apr_dbd.h"

find_missing()
{
	apr_status_t ret;
	apr_pool_t *p;
	apr_pool_t *tpool;
	int nrows;
    apr_dbd_t* handle1 = NULL;
    apr_dbd_t* handle2 = NULL;
    const apr_dbd_driver_t* driver = NULL;
    const char *dir,*filename;
    const char *fileCount;

    char *sql2;

    apr_dbd_results_t *res = NULL;
    apr_dbd_results_t *res1 = NULL;
    apr_dbd_row_t *row = NULL;
    apr_dbd_row_t *row1 = NULL;

	apr_pool_create(&p, NULL);


	ret = apr_dbd_init(p);
	ret = apr_dbd_get_driver(p, "sqlite3", &driver);

	ret = apr_dbd_open(driver, p, "slike_ext4.db", &handle1);
	ret = apr_dbd_open(driver, p, "slike.db", &handle2);

	const char *sql = "select * from photos;";
	apr_pool_create(&tpool, p);

	apr_dbd_select(driver, p, handle1, &res, sql, 0);
	while (apr_dbd_get_row(driver, tpool, res, &row, -1)==0)
	{
		dir = apr_dbd_get_entry(driver,row,2);
		filename = apr_dbd_get_entry(driver,row,3);
		sql2 =	 apr_psprintf(p, "select count(*) from photos where dir=\"%s\" and filename=\"%s\";",dir,filename);

		ret = apr_dbd_select(driver, p, handle2, &res1, sql2, 0);
		if (ret != 0)
		{
			printf("error select \n");
			return 1;
		}
		ret = apr_dbd_get_row(driver, tpool, res1, &row1, -1);
		fileCount = apr_dbd_get_entry(driver,row1,0);
		if (fileCount == NULL)
		{
			printf("error \n");
			return 1;
		}
		if (atoi(fileCount) == 0)
		{
			printf("Missing picture:%s - %s\n",dir,filename);
		}

		apr_pool_clear(tpool);
	}


	//rv = apr_dbd_query(driver, handle1, &nrows, sql);

	//ret = apr_dbd_query(driver, handle1, &nrows, sql);

	ret = apr_dbd_close(driver, handle1);
	ret = apr_dbd_close(driver, handle2);


	apr_pool_destroy(p);
}

