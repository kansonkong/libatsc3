//
// Created by Jason Justman on 8/10/20.
//

#ifndef LIBATSC3_ATSC3ASSERTS_H
#define LIBATSC3_ATSC3ASSERTS_H



#define ASSERT(cond,s) do { \
        if (!(cond)) { eprintf("%s: !! %s assert fail, line %d\n", __func__, s, __LINE__); \
            return -1; } \
        } while(0)

#define CHK_AR(ar,s) do { \
		if (ar) { eprintf("%s: !! %s, err %d, line %d\n", __func__, s, ar, __LINE__); \
			return -1; } \
		} while(0)
#define SHOW_AR(ar,s) do { \
		if (ar) { printf("%s: !! %s, err %d, line %d\n", __func__, s, ar, __LINE__); } \
		} while(0)


#endif //LIBATSC3_ATSC3ASSERTS_H
