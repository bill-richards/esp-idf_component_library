#ifndef __GSDC_UTILS_H__
#define __GSDC_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char * gsdc_string_utils_url_decode(char *str);

/* IMPORTANT: be sure to free() the returned string after use */
char * gsdc_string_utils_replace_substring(char* input_string, char* to_be_replaced, char* replace_with);

#ifdef __cplusplus
}
#endif

#endif // __GSDC_UTILS_H__

