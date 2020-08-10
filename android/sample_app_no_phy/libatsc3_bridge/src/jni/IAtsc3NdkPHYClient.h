//
// Created by Jason Justman on 8/10/20.
//

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_NO_PHY_IATSC3NDKPHYCLIENT_H
#define LIBATSC3_ANDROID_SAMPLE_APP_NO_PHY_IATSC3NDKPHYCLIENT_H

class IAtsc3NdkPHYClient {
public:
    virtual int Init() = 0;
    virtual int Open(int fd, int bus, int addr) = 0;
    virtual int Tune(int freqKhz, int plp) = 0;
    virtual int Stop()  = 0;
    virtual int Close()  = 0;

    virtual ~IAtsc3NdkPHYClient() {};

};


#endif //LIBATSC3_ANDROID_SAMPLE_APP_NO_PHY_IATSC3NDKPHYCLIENT_H
