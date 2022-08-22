#include "concrete-core-ffi.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

#include "utils.h"

void mul_cleartext_view_buffers_test(void) {
  // We generate the random sources
  DefaultEngine *engine = NULL;
  SeederBuilder *builder = get_best_seeder();

  int default_engine_ok = new_default_engine(builder, &engine);
  assert(default_engine_ok == 0);
  double variance = 0.000000001;

  // We generate the key
  size_t lwe_dimension = 10;
  LweSecretKey64 *sk = NULL;
  int sk_ok = default_engine_create_lwe_secret_key_u64(engine, lwe_dimension, &sk);
  assert(sk_ok == 0);

  // We generate the texts
  uint64_t *input_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));
  uint64_t *output_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));

  LweCiphertextView64 *input_ct_as_view = NULL;
  int input_ct_view_ok = default_engine_create_lwe_ciphertext_view_u64(
      engine, input_ct_buffer, lwe_dimension + 1, &input_ct_as_view);
  assert(input_ct_view_ok == 0);

  LweCiphertextMutView64 *input_ct_as_mut_view = NULL;
  int input_ct_mut_view_ok = default_engine_create_lwe_ciphertext_mut_view_u64(
      engine, input_ct_buffer, lwe_dimension + 1, &input_ct_as_mut_view);
  assert(input_ct_mut_view_ok == 0);

  LweCiphertextView64 *output_ct_as_view = NULL;
  int output_ct_as_view_ok = default_engine_create_lwe_ciphertext_view_u64(
      engine, output_ct_buffer, lwe_dimension + 1, &output_ct_as_view);
  assert(output_ct_as_view_ok == 0);

  LweCiphertextMutView64 *output_ct_as_mut_view = NULL;
  int output_ct_mut_view_ok = default_engine_create_lwe_ciphertext_mut_view_u64(
      engine, output_ct_buffer, lwe_dimension + 1, &output_ct_as_mut_view);
  assert(output_ct_mut_view_ok == 0);

  uint64_t plaintext = ((uint64_t)2) << SHIFT;

  uint64_t integer_multiplier = 3;

  // We encrypt the plaintext
  int encrypt_ok = default_engine_discard_encrypt_lwe_ciphertext_u64_view_buffers(
      engine, sk, input_ct_as_mut_view, plaintext, variance);
  assert(encrypt_ok == 0);

  int mul_ok = default_engine_discard_mul_lwe_ciphertext_cleartext_u64_view_buffers(
      engine, output_ct_as_mut_view, input_ct_as_view, integer_multiplier);
  assert(mul_ok == 0);

  // We decrypt the plaintext
  uint64_t output = -1;
  int decrypt_ok = default_engine_decrypt_lwe_ciphertext_u64_view_buffers(
      engine, sk, output_ct_as_view, &output);
  assert(decrypt_ok == 0);

  // We check that the output are the same
  double expected = ((double)plaintext * (double)integer_multiplier) / pow(2, SHIFT);
  double obtained = (double)output / pow(2, SHIFT);
  printf("Comparing output. Expected %f, Obtained %f\n", expected, obtained);
  double abs_diff = abs(obtained - expected);
  double rel_error = abs_diff / fmax(expected, obtained);
  assert(rel_error < 0.001);

  // We deallocate the objects
  destroy_lwe_secret_key_u64(sk);
  destroy_lwe_ciphertext_view_u64(input_ct_as_view);
  destroy_lwe_ciphertext_mut_view_u64(input_ct_as_mut_view);
  destroy_lwe_ciphertext_view_u64(output_ct_as_view);
  destroy_lwe_ciphertext_mut_view_u64(output_ct_as_mut_view);
  destroy_default_engine(engine);
  destroy_seeder_builder(builder);
  free(input_ct_buffer);
  free(output_ct_buffer);
}

void mul_cleartext_unchecked_view_buffers_test(void) {
  // We generate the random sources
  DefaultEngine *engine = NULL;
  SeederBuilder *builder = get_best_seeder_unchecked();

  int default_engine_ok = new_default_engine_unchecked(builder, &engine);
  assert(default_engine_ok == 0);
  double variance = 0.000000001;

  // We generate the key
  size_t lwe_dimension = {10};
  LweSecretKey64 *sk = NULL;
  int sk_ok = default_engine_create_lwe_secret_key_unchecked_u64(engine, lwe_dimension, &sk);
  assert(sk_ok == 0);

  // We generate the texts
  uint64_t *input_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));
  uint64_t *output_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));

  LweCiphertextView64 *input_ct_as_view = NULL;
  int input_ct_view_ok = default_engine_create_lwe_ciphertext_view_unchecked_u64(
      engine, input_ct_buffer, lwe_dimension + 1, &input_ct_as_view);
  assert(input_ct_view_ok == 0);

  LweCiphertextMutView64 *input_ct_as_mut_view = NULL;
  int input_ct_mut_view_ok = default_engine_create_lwe_ciphertext_mut_view_unchecked_u64(
      engine, input_ct_buffer, lwe_dimension + 1, &input_ct_as_mut_view);
  assert(input_ct_mut_view_ok == 0);

  LweCiphertextView64 *output_ct_as_view = NULL;
  int output_ct_as_view_ok = default_engine_create_lwe_ciphertext_view_unchecked_u64(
      engine, output_ct_buffer, lwe_dimension + 1, &output_ct_as_view);
  assert(output_ct_as_view_ok == 0);

  LweCiphertextMutView64 *output_ct_as_mut_view = NULL;
  int output_ct_mut_view_ok = default_engine_create_lwe_ciphertext_mut_view_unchecked_u64(
      engine, output_ct_buffer, lwe_dimension + 1, &output_ct_as_mut_view);
  assert(output_ct_mut_view_ok == 0);

  uint64_t plaintext = ((uint64_t)2) << SHIFT;

  uint64_t integer_multiplier = 3;

  // We encrypt the plaintext
  int encrypt_ok = default_engine_discard_encrypt_lwe_ciphertext_unchecked_u64_view_buffers(
      engine, sk, input_ct_as_mut_view, plaintext, variance);
  assert(encrypt_ok == 0);

  int mul_ok = default_engine_discard_mul_lwe_ciphertext_cleartext_unchecked_u64_view_buffers(
      engine, output_ct_as_mut_view, input_ct_as_view, integer_multiplier);
  assert(mul_ok == 0);

  // We decrypt the plaintext
  uint64_t output = -1;
  int decrypt_ok = default_engine_decrypt_lwe_ciphertext_unchecked_u64_view_buffers(
      engine, sk, output_ct_as_view, &output);
  assert(decrypt_ok == 0);

  // We check that the output are the same
  double expected = ((double)plaintext * (double)integer_multiplier) / pow(2, SHIFT);
  double obtained = (double)output / pow(2, SHIFT);
  printf("Comparing output. Expected %f, Obtained %f\n", expected, obtained);
  double abs_diff = abs(obtained - expected);
  double rel_error = abs_diff / fmax(expected, obtained);
  assert(rel_error < 0.001);

  // We deallocate the objects
  destroy_lwe_secret_key_unchecked_u64(sk);
  destroy_lwe_ciphertext_view_unchecked_u64(input_ct_as_view);
  destroy_lwe_ciphertext_mut_view_unchecked_u64(input_ct_as_mut_view);
  destroy_lwe_ciphertext_view_unchecked_u64(output_ct_as_view);
  destroy_lwe_ciphertext_mut_view_unchecked_u64(output_ct_as_mut_view);
  destroy_default_engine_unchecked(engine);
  destroy_seeder_builder_unchecked(builder);
  free(input_ct_buffer);
  free(output_ct_buffer);
}

void mul_cleartext_raw_ptr_buffers_test(void) {
  // We generate the random sources
  DefaultEngine *engine = NULL;
  SeederBuilder *builder = get_best_seeder();

  int default_engine_ok = new_default_engine(builder, &engine);
  assert(default_engine_ok == 0);
  double variance = 0.000000001;

  // We generate the key
  size_t lwe_dimension = {10};
  LweSecretKey64 *sk = NULL;
  int sk_ok = default_engine_create_lwe_secret_key_u64(engine, lwe_dimension, &sk);
  assert(sk_ok == 0);

  // We generate the texts
  uint64_t *input_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));
  uint64_t *output_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));

  uint64_t plaintext = ((uint64_t)2) << SHIFT;

  uint64_t integer_multiplier = 3;

  // We encrypt the plaintext
  int encrypt_ok = default_engine_discard_encrypt_lwe_ciphertext_u64_raw_ptr_buffers(
      engine, sk, input_ct_buffer, plaintext, variance);
  assert(encrypt_ok == 0);

  int mul_ok = default_engine_discard_mul_lwe_ciphertext_cleartext_u64_raw_ptr_buffers(
      engine, output_ct_buffer, input_ct_buffer, lwe_dimension, integer_multiplier);
  assert(mul_ok == 0);

  // We decrypt the plaintext
  uint64_t output = -1;
  int decrypt_ok = default_engine_decrypt_lwe_ciphertext_u64_raw_ptr_buffers(
      engine, sk, output_ct_buffer, &output);
  assert(decrypt_ok == 0);

  // We check that the output are the same
  double expected = ((double)plaintext * (double)integer_multiplier) / pow(2, SHIFT);
  double obtained = (double)output / pow(2, SHIFT);
  printf("Comparing output. Expected %f, Obtained %f\n", expected, obtained);
  double abs_diff = abs(obtained - expected);
  double rel_error = abs_diff / fmax(expected, obtained);
  assert(rel_error < 0.001);

  // We deallocate the objects
  destroy_lwe_secret_key_u64(sk);
  destroy_default_engine(engine);
  destroy_seeder_builder(builder);
  free(input_ct_buffer);
  free(output_ct_buffer);
}

void mul_cleartext_unchecked_raw_ptr_buffers_test(void) {
  // We generate the random sources
  DefaultEngine *engine = NULL;
  SeederBuilder *builder = get_best_seeder_unchecked();

  int default_engine_ok = new_default_engine_unchecked(builder, &engine);
  assert(default_engine_ok == 0);
  double variance = 0.000000001;

  // We generate the key
  size_t lwe_dimension = {10};
  LweSecretKey64 *sk = NULL;
  int sk_ok = default_engine_create_lwe_secret_key_unchecked_u64(engine, lwe_dimension, &sk);
  assert(sk_ok == 0);

  // We generate the texts
  uint64_t *input_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));
  uint64_t *output_ct_buffer = aligned_alloc(U64_ALIGNMENT, sizeof(uint64_t) * (lwe_dimension + 1));

  uint64_t plaintext = ((uint64_t)2) << SHIFT;

  uint64_t integer_multiplier = 3;

  // We encrypt the plaintext
  int encrypt_ok = default_engine_discard_encrypt_lwe_ciphertext_unchecked_u64_raw_ptr_buffers(
      engine, sk, input_ct_buffer, plaintext, variance);
  assert(encrypt_ok == 0);

  int mul_ok = default_engine_discard_mul_lwe_ciphertext_cleartext_unchecked_u64_raw_ptr_buffers(
      engine, output_ct_buffer, input_ct_buffer, lwe_dimension, integer_multiplier);
  assert(mul_ok == 0);

  // We decrypt the plaintext
  uint64_t output = -1;
  int decrypt_ok = default_engine_decrypt_lwe_ciphertext_unchecked_u64_raw_ptr_buffers(
      engine, sk, output_ct_buffer, &output);
  assert(decrypt_ok == 0);

  // We check that the output are the same
  double expected = ((double)plaintext * (double)integer_multiplier) / pow(2, SHIFT);
  double obtained = (double)output / pow(2, SHIFT);
  printf("Comparing output. Expected %f, Obtained %f\n", expected, obtained);
  double abs_diff = abs(obtained - expected);
  double rel_error = abs_diff / fmax(expected, obtained);
  assert(rel_error < 0.001);

  // We deallocate the objects
  destroy_lwe_secret_key_unchecked_u64(sk);
  destroy_default_engine_unchecked(engine);
  destroy_seeder_builder_unchecked(builder);
  free(input_ct_buffer);
  free(output_ct_buffer);
}

int main(void) {
  mul_cleartext_view_buffers_test();
  mul_cleartext_unchecked_view_buffers_test();
  mul_cleartext_raw_ptr_buffers_test();
  mul_cleartext_unchecked_raw_ptr_buffers_test();
  return EXIT_SUCCESS;
}
