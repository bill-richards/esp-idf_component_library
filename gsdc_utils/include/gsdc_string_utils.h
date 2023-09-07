#ifndef __GSDC_UTILS_H__
#define __GSDC_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns a url-decoded version of input_string
 * IMPORTANT: be sure to free() the returned string after use 
 * @param input_string (char *) the string to be decoded
 * @returns A decoded version of input_string
 */
char * gsdc_string_utils_url_decode(char * str);

/** 
 * @brief Returns a url-encoded version of input_string
 * IMPORTANT: be sure to free() the returned string after use 
 * @param input_string (char *) the string to be encoded  
 * @returns An encoded version of input_string
 */
char * gsdc_string_utils_url_encode(char * str);

/**
 * @brief Replaces substring (to_be_replaced) with substring (replace_with) within the input string (input_string)
 * IMPORTANT: be sure to free() the returned string after use 
 * @param input_string (char *) the string containing the substring to be replced
 * @param to_be_replaced (char *) the substring to be replaced
 * @param replace_with (char *) the substring to replace with
 */
char * gsdc_string_utils_replace_substring(char * input_string, char * to_be_replaced, char * replace_with);

char from_hex(char ch);
char to_hex(char code);

#ifdef __cplusplus
}
#endif

#endif // __GSDC_UTILS_H__

