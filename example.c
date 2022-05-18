#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"

int main(void)
{
    // A short example on how to use the MD5 library
    // ---------------------------------------------

    // First, before we can compute anything, we need a properly initialized MD5
    // context.
    MD5_CTX md5_ctx;

    // It is of paramount importance to call an initialization function,
    // otherwise the contents will not be properly defined.
    MD5_Init(&md5_ctx);

    // Now, we can compute the hash from arbitrary blocks of bytes using the
    // Update method:
    const char string[] = "pajdfojbadfjkd";
    MD5_Update(&md5_ctx, string, strlen(string));

    // Sometimes we would like to add more data, perhaps because we are reading
    // a large file that cannot fit into the memory. No problem, we can call
    // ‹MD5_Update()› for each of these blocks as many times as we want.

    // Finally, when we are ready to extract the hash, we need a piece of memory
    // to store it. 16 bytes will do:
    unsigned char md5_hash[16];
    MD5_Final(md5_hash, &md5_ctx);

    // Here, <md5_hash> is an array of *bytes* (not necessarily printable
    // characters!) that comprise the resulting hash.

    for (int i = 0; i < sizeof(md5_hash); ++i) {
        printf("%02x" /* How do we print a byte in hex? */ , md5_hash[i]);
    }

    putchar('\n');
    printf("%lu\n", strlen("5603c5e1a4e5227f3cee312ad611a095"));

    // NOTE: You can reuse ‹md5_ctx› after ‹MD5_Final()›, but you *must* call
    // ‹MD5_Init()› again!

    return EXIT_SUCCESS;
}
