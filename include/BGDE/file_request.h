#ifndef BGDE_FILE_REQUEST_H
#define BGDE_FILE_REQUEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* actual file request function provided by bgde_file_request library */
void bgde_file_request(const char *request, char *response_buf, int buf_size);

#ifdef __cplusplus
}
#endif

#endif
