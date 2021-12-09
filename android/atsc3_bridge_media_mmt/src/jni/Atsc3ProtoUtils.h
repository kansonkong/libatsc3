#ifndef LIBATSC3_ATSC3_NDK_MEDIA_MMT_PROTO_UTILS_H
#define LIBATSC3_ATSC3_NDK_MEDIA_MMT_PROTO_UTILS_H

template <typename T>
inline jbyteArray protoNewByteArrayFromObject(JNIEnv *env, const T& obj) {
    size_t bufferSize = obj.ByteSizeLong();
    char *p = (char *) malloc(bufferSize);
    obj.SerializeToArray(p, bufferSize);

    jbyteArray jArray = env->NewByteArray(bufferSize);
    env->SetByteArrayRegion(jArray, 0, bufferSize, (jbyte*) p);
    free(p);

    return jArray;
}

#endif //LIBATSC3_ATSC3_NDK_MEDIA_MMT_PROTO_UTILS_H
