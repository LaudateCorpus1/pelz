/*
 * main.c
 *
 * Main function to do unit testing for PELZ.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "enclave_test_suite.h"
#include "util_test_suite.h"
#include "pelz_json_parser_test_suite.h"
#include <pelz_log.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "sgx_urts.h"
#include "pelz_enclave.h"
#include "pelz_enclave_u.h"
sgx_enclave_id_t eid = 0;

#define ENCLAVE_PATH "sgx/pelz_enclave.signed.so"

// Blank Suite's init and cleanup code
int init_suite(void)
{
  return (0);
}

int clean_suite(void)
{
  return (0);
}

//Main function for the unit testing of the Pelz Service application
int main(int argc, char **argv)
{
  const char *key_file_id[6] = { "test/key1.txt", "test/key2.txt", "test/key3.txt", "test/key4.txt", "test/key5.txt",
    "test/key6.txt"
  };
  const char *key[6] = { "KIENJCDNHVIJERLMALIDFEKIUFDALJFG", "KALIENGVBIZSAIXKDNRUEHFMDDUHVKAN", "HVIJERLMALIDFKDN",
    "NGVBIZSAIXKDNRUE", "EKIUFDALVBIZSAIXKDNRUEHV", "ALIENGVBCDNHVIJESAIXEKIU"
  };

  set_app_name("pelz");
  set_app_version("0.0.0");
  set_applog_max_msg_len(1024);
  set_applog_path("./test/log/pelz.log");
  set_applog_severity_threshold(LOG_DEBUG);

  for (int i = 0; i < 6; i++)
  {
    FILE *fp = fopen(key_file_id[i], "w");

    fprintf(fp, "%s", key[i]);
    fclose(fp);
  }

  pelz_log(LOG_DEBUG, "Start Unit Test");
  // Initialize CUnit test registry
  if (CUE_SUCCESS != CU_initialize_registry())
  {
    return CU_get_error();
  }

  sgx_create_enclave(ENCLAVE_PATH, 0, NULL, NULL, &eid, NULL);

  // Add enclave suite ---- tests key table init/destroy/delete and pelz_request_handler functions 
  CU_pSuite enclave_Suite = NULL;

  enclave_Suite = CU_add_suite("Enclave Suite", init_suite, clean_suite);
  if (NULL == enclave_Suite)
  {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (enclave_suite_add_tests(enclave_Suite))
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Add utility suite --- tests util/util.h functions
  CU_pSuite utility_Suite = NULL;

  utility_Suite = CU_add_suite("Utility Suite", init_suite, clean_suite);
  if (NULL == utility_Suite)
  {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (utility_suite_add_tests(utility_Suite))
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Add pelz json parser suite ---- tests pelz json parser encrypt_parse/decrypt_parse/request_decode/message_encode/error_message_encode functions
  CU_pSuite pelz_json_parser_Suite = NULL;

  pelz_json_parser_Suite = CU_add_suite("Pelz JSON Parser Suite", init_suite, clean_suite);
  if (NULL == pelz_json_parser_Suite)
  {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (pelz_json_parser_suite_add_tests(pelz_json_parser_Suite))
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  pelz_log(LOG_DEBUG, "Run tests using basic interface");
  // Run tests using basic interface
  CU_basic_run_tests();
  //CU_console_run_tests();
  //CU_automated_run_tests();

  sgx_destroy_enclave(eid);
  for (int i = 0; i < 6; i++)
  {
    remove(key_file_id[i]);
  }

  pelz_log(LOG_DEBUG, "Clean up registry and return");
  // Clean up registry and return
  CU_cleanup_registry();
  return CU_get_error();
}
