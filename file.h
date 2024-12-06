#ifndef FILE_H
#define FILE_H

/*!
 * Fungsi untuk membaca file
 * @param directory Direktori dari file yang akan dibaca
 * @param file_name Nama dari file yang akan dibaca
 * @return File descriptor
 */
extern int get_file_descriptor(const char *directory, const char *file_name);

/*!
 * Fungsi untuk membaca dan mengekstrak file type
 * @param file_name Nama dari file yang akan diekstrak file type-nya
 * @return File type
 */
extern const char *get_file_extension(const char *file_name);

/*!
 * Fungsi untuk mendapatkan MIME saat file type diberikan
 * @param file_ext File ekstension yang sudah diekstrak
 * @return MIME
 */
extern const char *get_mime_type(const char *file_ext);

/*!
 * Untuk decode URL
 * Apabila requested URL memiliki hexadecimal value, itu akan di decode menjadi UNICODE format
 * @param src Requested URL
 * @return URL yang sudah di-decode
 */
extern char *url_decode(const char *src);

#endif // !FILE_H
