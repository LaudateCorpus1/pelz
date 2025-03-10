/*
 * pelz_json_parser_suite.c
 */

#include "pelz_json_parser_test_suite.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include <charbuf.h>
#include <pelz_log.h>
#include <pelz_request_handler.h>

// Adds all key table tests to main test runner.
int pelz_json_parser_suite_add_tests(CU_pSuite suite)
{
  if (NULL == CU_add_test(suite, "Test JSON Encryption Request Parser", test_encrypt_parser))
  {
    return (1);
  }
  if (NULL == CU_add_test(suite, "Test JSON Decryption Request Parser", test_decrypt_parser))
  {
    return (1);
  }
  if (NULL == CU_add_test(suite, "Test Decoding of JSON formatted Request", test_request_decoder))
  {
    return (1);
  }
  if (NULL == CU_add_test(suite, "Test Encoding of JSON formatted Response Message", test_message_encoder))
  {
    return (1);
  }
  if (NULL == CU_add_test(suite, "Test Encoding of JSON formatted Error Message", test_error_message_encoder))
  {
    return (1);
  }
  return (0);
}

void test_encrypt_parser(void)
{
  cJSON *json;
  charbuf key_id;
  charbuf data;

  //Valid Test Values
  const char *json_key_id = "file:/test/key1.txt";
  unsigned int json_key_id_len = 19;
  const char *enc_data = "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVoxMjM0NTY=\n";
  unsigned int enc_data_len = 45;
  const char *dec_data = "SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\n";
  unsigned int dec_data_len = 57;

  //Building of a standard valid JSON request
  json = cJSON_CreateObject();
  cJSON_AddItemToObject(json, "request_type", cJSON_CreateNumber(1));
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateString(json_key_id));
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));
  cJSON_AddItemToObject(json, "enc_data", cJSON_CreateString(enc_data));
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(enc_data_len));
  cJSON_AddItemToObject(json, "dec_data", cJSON_CreateString(dec_data));
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(dec_data_len));

  //Test standard valid JSON request
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 0);
  CU_ASSERT(key_id.len == json_key_id_len);
  CU_ASSERT(memcmp(key_id.chars, json_key_id, key_id.len) == 0);
  CU_ASSERT(data.len == enc_data_len);
  CU_ASSERT(memcmp(data.chars, enc_data, data.len) == 0);
  free_charbuf(&key_id);
  free_charbuf(&data);

  //Test check of JSON request hasObject
  cJSON_DeleteItemFromObject(json, "key_id");
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateString(json_key_id));

  cJSON_DeleteItemFromObject(json, "key_id_len");
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));

  cJSON_DeleteItemFromObject(json, "enc_data");
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "enc_data", cJSON_CreateString(enc_data));

  cJSON_DeleteItemFromObject(json, "enc_data_len");
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(enc_data_len));

  cJSON_DeleteItemFromObject(json, "dec_data");
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 0);
  free_charbuf(&key_id);
  free_charbuf(&data);
  cJSON_AddItemToObject(json, "dec_data", cJSON_CreateString(dec_data));

  cJSON_DeleteItemFromObject(json, "dec_data_len");
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 0);
  free_charbuf(&key_id);
  free_charbuf(&data);
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(dec_data_len));

  //Test check of JSON request isNumber
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateString("19"));
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));

  cJSON_DeleteItemFromObject(json, "enc_data_len");
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateString("45"));
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "enc_data_len");
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(enc_data_len));

  //Test check of JSON request isString
  cJSON_DeleteItemFromObject(json, "key_id");
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateNumber(5482));
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "key_id");
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateString(json_key_id));

  cJSON_DeleteItemFromObject(json, "enc_data");
  cJSON_AddItemToObject(json, "enc_data", cJSON_CreateNumber(6842));
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "enc_data");
  cJSON_AddItemToObject(json, "enc_data", cJSON_CreateString(enc_data));

  //Test check of JSON request string length match
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(20));
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));

  cJSON_DeleteItemFromObject(json, "enc_data_len");
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(50));
  CU_ASSERT(encrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "enc_data_len");
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(enc_data_len));

  //Clean-up JSON
  cJSON_Delete(json);
}

void test_decrypt_parser(void)
{
  cJSON *json;
  charbuf key_id;
  charbuf data;

  //Valid Test Values
  const char *json_key_id = "file:/test/key1.txt";
  unsigned int json_key_id_len = 19;
  const char *enc_data = "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVoxMjM0NTY=\n";
  unsigned int enc_data_len = 45;
  const char *dec_data = "SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\n";
  unsigned int dec_data_len = 57;

  //Building of a standard valid JSON request
  json = cJSON_CreateObject();
  cJSON_AddItemToObject(json, "request_type", cJSON_CreateNumber(2));
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateString(json_key_id));
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));
  cJSON_AddItemToObject(json, "enc_data", cJSON_CreateString(enc_data));
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(enc_data_len));
  cJSON_AddItemToObject(json, "dec_data", cJSON_CreateString(dec_data));
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(dec_data_len));

  //Test standard valid JSON request
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 0);
  CU_ASSERT(key_id.len == json_key_id_len);
  CU_ASSERT(memcmp(key_id.chars, json_key_id, key_id.len) == 0);
  CU_ASSERT(data.len == dec_data_len);
  CU_ASSERT(memcmp(data.chars, dec_data, data.len) == 0);
  free_charbuf(&key_id);
  free_charbuf(&data);

  //Test check of JSON request hasObject
  cJSON_DeleteItemFromObject(json, "key_id");
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateString(json_key_id));

  cJSON_DeleteItemFromObject(json, "key_id_len");
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));

  cJSON_DeleteItemFromObject(json, "dec_data");
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "dec_data", cJSON_CreateString(dec_data));

  cJSON_DeleteItemFromObject(json, "dec_data_len");
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(dec_data_len));

  cJSON_DeleteItemFromObject(json, "enc_data");
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 0);
  free_charbuf(&key_id);
  free_charbuf(&data);
  cJSON_AddItemToObject(json, "enc_data", cJSON_CreateString(enc_data));

  cJSON_DeleteItemFromObject(json, "enc_data_len");
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 0);
  free_charbuf(&key_id);
  free_charbuf(&data);
  cJSON_AddItemToObject(json, "enc_data_len", cJSON_CreateNumber(enc_data_len));

  //Test check of JSON request isNumber
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateString("19"));
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));

  cJSON_DeleteItemFromObject(json, "dec_data_len");
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateString("57"));
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "dec_data_len");
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(dec_data_len));

  //Test check of JSON request isString
  cJSON_DeleteItemFromObject(json, "key_id");
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateNumber(5482));
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "key_id");
  cJSON_AddItemToObject(json, "key_id", cJSON_CreateString(json_key_id));

  cJSON_DeleteItemFromObject(json, "dec_data");
  cJSON_AddItemToObject(json, "dec_data", cJSON_CreateNumber(6842));
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "dec_data");
  cJSON_AddItemToObject(json, "dec_data", cJSON_CreateString(dec_data));

  //Test check of JSON request string length match
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(20));
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "key_id_len");
  cJSON_AddItemToObject(json, "key_id_len", cJSON_CreateNumber(json_key_id_len));

  cJSON_DeleteItemFromObject(json, "dec_data_len");
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(50));
  CU_ASSERT(decrypt_parser(json, &key_id, &data) == 1);
  cJSON_DeleteItemFromObject(json, "dec_data_len");
  cJSON_AddItemToObject(json, "dec_data_len", cJSON_CreateNumber(dec_data_len));

  //Clean-up JSON
  cJSON_Delete(json);
}

void test_request_decoder(void)
{
  charbuf request;
  char *tmp;
  RequestType request_type;
  charbuf key_id;
  charbuf data;
  cJSON *json_enc;
  cJSON *json_dec;

  const char *invalid_request[4] = {
    "{\"key_id_len\": 28, \"key_id\": \"file:/test/testkeys/key2.txt\"}",
    "{\"request_type\": \"one\"}", "{\"request_type\": 0}", "{\"request_type\": 3}"
  };
  const char *json_key_id[6] = {
    "file:/test/key1.txt", "file:/test/key2.txt", "file:/test/key3.txt",
    "file:/test/key4.txt", "file:/test/key5.txt", "file:/test/key6.txt"
  };
  unsigned int json_key_id_len = 19;

  const char *enc_data[6] = {
    "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVoxMjM0NTY=\n", "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXoxMjM0NTY=\n",
    "QUJDREVGR0hJSktMTU5PUFFSU1RVVldY\n", "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4\n",
    "QUJDREVGR0hJSktMTU5PUA==\n", "YWJjZGVmZ2hpamtsbW5vcA==\n"
  };
  unsigned int enc_data_len[6] = {
    45, 45, 33, 33, 25, 25
  };
  const char *dec_data[6] = {
    "SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\n",
    "txQhouR/i5+lycST2QXuN39gQqVQYVy9mWf3RdSdXfZNUy4CsQqwBg==\n",
    "+n4yYCmMXyNbyEtsJuFlBtkCbVDXhjVRON/osW5dbz8=\n", "BtIjIgvCaVBwUi5jTOZyIx2yJamqvrR0BZWLFVufz9w=\n",
    "K2YZ6dTyLpmXRseKg+wwlPUZCnFmYBEn\n", "K6iS75+hIrCNuo9LWeEhjDQ2L9miNR07\n"
  };
  unsigned int dec_data_len[6] = {
    57, 57, 45, 45, 33, 33
  };

  pelz_log(LOG_DEBUG, "Start Request Decoder Test");
  //Test Invalid Requests with bad request_types
  for (int i = 0; i < 4; i++)
  {
    request = new_charbuf(strlen(invalid_request[i]));
    memcpy(request.chars, invalid_request[i], request.len);
    CU_ASSERT(request_decoder(request, &request_type, &key_id, &data) == 1);
    free_charbuf(&request);
    request_type = REQ_UNK;
  }

  //Building of the json request and most combinations
  json_enc = cJSON_CreateObject();
  json_dec = cJSON_CreateObject();
  cJSON_AddItemToObject(json_enc, "request_type", cJSON_CreateNumber(1));
  cJSON_AddItemToObject(json_dec, "request_type", cJSON_CreateNumber(2));

  tmp = cJSON_PrintUnformatted(json_enc);
  request = new_charbuf(strlen(tmp));
  memcpy(request.chars, tmp, request.len);
  free(tmp);
  CU_ASSERT(request_decoder(request, &request_type, &key_id, &data) == 1);
  free_charbuf(&request);
  request_type = REQ_UNK;

  tmp = cJSON_PrintUnformatted(json_dec);
  request = new_charbuf(strlen(tmp));
  memcpy(request.chars, tmp, request.len);
  free(tmp);
  CU_ASSERT(request_decoder(request, &request_type, &key_id, &data) == 1);
  free_charbuf(&request);
  request_type = REQ_UNK;

  for (int i = 0; i < 6; i++)
  {
    cJSON_AddItemToObject(json_enc, "key_id", cJSON_CreateString(json_key_id[i]));
    cJSON_AddItemToObject(json_enc, "key_id_len", cJSON_CreateNumber(json_key_id_len));
    cJSON_AddItemToObject(json_dec, "key_id", cJSON_CreateString(json_key_id[i]));
    cJSON_AddItemToObject(json_dec, "key_id_len", cJSON_CreateNumber(json_key_id_len));
    cJSON_AddItemToObject(json_enc, "enc_data", cJSON_CreateString(enc_data[i]));
    cJSON_AddItemToObject(json_dec, "dec_data", cJSON_CreateString(dec_data[i]));
    cJSON_AddItemToObject(json_enc, "enc_data_len", cJSON_CreateNumber(enc_data_len[i]));
    cJSON_AddItemToObject(json_dec, "dec_data_len", cJSON_CreateNumber(dec_data_len[i]));

    //Creating the request charbuf for the JSON then testing request_decoder for encryption
    tmp = cJSON_PrintUnformatted(json_enc);
    request = new_charbuf(strlen(tmp));
    memcpy(request.chars, tmp, request.len);
    free(tmp);
    CU_ASSERT(request_decoder(request, &request_type, &key_id, &data) == 0);
    CU_ASSERT(request_type == REQ_ENC);
    CU_ASSERT(key_id.len == json_key_id_len);
    CU_ASSERT(memcmp(key_id.chars, json_key_id[i], key_id.len) == 0);
    CU_ASSERT(data.len == enc_data_len[i]);
    CU_ASSERT(memcmp(data.chars, enc_data[i], data.len) == 0);
    free_charbuf(&request);
    request_type = REQ_UNK;
    free_charbuf(&key_id);
    free_charbuf(&data);

    //Creating the request charbuf for the JSON then testing request_decoder for decryption
    tmp = cJSON_PrintUnformatted(json_dec);
    request = new_charbuf(strlen(tmp));
    memcpy(request.chars, tmp, request.len);
    free(tmp);
    CU_ASSERT(request_decoder(request, &request_type, &key_id, &data) == 0);
    CU_ASSERT(request_type == REQ_DEC);
    CU_ASSERT(key_id.len == json_key_id_len);
    CU_ASSERT(memcmp(key_id.chars, json_key_id[i], key_id.len) == 0);
    CU_ASSERT(data.len == dec_data_len[i]);
    CU_ASSERT(memcmp(data.chars, dec_data[i], data.len) == 0);
    free_charbuf(&request);
    request_type = REQ_UNK;
    free_charbuf(&key_id);
    free_charbuf(&data);

    //Free the cJSON Objects to allow the addition of the next Object per the loop
    cJSON_DeleteItemFromObject(json_dec, "dec_data");
    cJSON_DeleteItemFromObject(json_dec, "dec_data_len");
    cJSON_DeleteItemFromObject(json_enc, "enc_data");
    cJSON_DeleteItemFromObject(json_enc, "enc_data_len");
    cJSON_DeleteItemFromObject(json_enc, "key_id");
    cJSON_DeleteItemFromObject(json_enc, "key_id_len");
    cJSON_DeleteItemFromObject(json_dec, "key_id");
    cJSON_DeleteItemFromObject(json_dec, "key_id_len");
  }

  cJSON_Delete(json_enc);
  cJSON_Delete(json_dec);
}

void test_message_encoder(void)
{
  charbuf key_id;
  charbuf data;
  charbuf message;
  const char *test[5] = { "file:/test/key1.txt", "test/key1.txt", "file", "anything", "" };
  const char *valid_enc_message[5] =
    { "{\"key_id\":\"file:/test/key1.txt\",\"key_id_len\":19,\"enc_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"enc_out_len\":57}",
    "{\"key_id\":\"test/key1.txt\",\"key_id_len\":13,\"enc_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"enc_out_len\":57}",
    "{\"key_id\":\"file\",\"key_id_len\":4,\"enc_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"enc_out_len\":57}",
    "{\"key_id\":\"anything\",\"key_id_len\":8,\"enc_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"enc_out_len\":57}",
    "{\"key_id\":\"\",\"key_id_len\":0,\"enc_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"enc_out_len\":57}"
  };
  const char *valid_dec_message[5] =
    { "{\"key_id\":\"file:/test/key1.txt\",\"key_id_len\":19,\"dec_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"dec_out_len\":57}",
    "{\"key_id\":\"test/key1.txt\",\"key_id_len\":13,\"dec_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"dec_out_len\":57}",
    "{\"key_id\":\"file\",\"key_id_len\":4,\"dec_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"dec_out_len\":57}",
    "{\"key_id\":\"anything\",\"key_id_len\":8,\"dec_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"dec_out_len\":57}",
    "{\"key_id\":\"\",\"key_id_len\":0,\"dec_out\":\"SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\\n\",\"dec_out_len\":57}"
  };

  //Start Message Encoder Test
  pelz_log(LOG_DEBUG, "Start Message Encoder Test");

  data = new_charbuf(57);
  memcpy(data.chars, "SwqqSZbNtN2SOfKGtE2jfklrcARSCZE9Tdl93pggkIsRkY3MrjevmQ==\n", data.len);
  key_id = new_charbuf(strlen(test[0]));
  memcpy(key_id.chars, test[0], key_id.len);
  CU_ASSERT(message_encoder(REQ_UNK, key_id, data, &message) == 1);
  free_charbuf(&key_id);

  for (int i = 0; i < 5; i++)
  {
    key_id = new_charbuf(strlen(test[i]));
    memcpy(key_id.chars, test[i], key_id.len);
    CU_ASSERT(message_encoder(REQ_ENC, key_id, data, &message) == 0);
    CU_ASSERT(memcmp(message.chars, valid_enc_message[i], message.len) == 0);
    free_charbuf(&message);
    CU_ASSERT(message_encoder(REQ_DEC, key_id, data, &message) == 0);
    CU_ASSERT(memcmp(message.chars, valid_dec_message[i], message.len) == 0);
    free_charbuf(&message);
    free_charbuf(&key_id);
  }
  free_charbuf(&data);
}

void test_error_message_encoder(void)
{
  pelz_log(LOG_DEBUG, "Test err msg");
  const char *err_msg[5] = {
    "Missing Data", "missing data", "akdifid", "Error", "Any message"
  };
  charbuf message;

  for (int i = 0; i < 5; i++)
  {
    CU_ASSERT(error_message_encoder(&message, err_msg[i]) == 0);
    pelz_log(LOG_DEBUG, "Error Message: %.*s", message.len, message.chars);
    free_charbuf(&message);
  }
}
