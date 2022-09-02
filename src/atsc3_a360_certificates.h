/*
  atsc3_a360_certificates.h

  ................. 2022-09-01


 ./openssl x509 -in root.pem  -noout -text

Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number:
            7d:6c:fc:1a:56:df:aa:89:73:82:f9:a6:cb:04:73:3f
        Signature Algorithm: sha256WithRSAEncryption
        Issuer: C = US, O = Pearl TV, OU = Root CA-1, CN = ATSC3 Root-CA
        Validity
            Not Before: Apr 19 00:00:00 2018 GMT
            Not After : Apr 18 23:59:59 2068 GMT
        Subject: C = US, O = Pearl TV, OU = Root CA-1, CN = ATSC3 Root-CA
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                RSA Public-Key: (4096 bit)
                Modulus:
                    00:e3:04:9a:cd:5c:ca:ba:4c:4c:ba:74:06:83:8b:
                    94:6b:da:73:52:a6:db:e4:6e:86:b6:a5:59:28:4b:
                    0a:02:0f:af:8e:27:95:0f:32:47:25:16:c7:48:3d:
                    f1:94:9c:48:7f:8d:8c:3f:43:07:0b:36:b9:f0:a2:
                    2a:45:57:df:99:0d:b0:23:cf:fc:57:d1:90:ba:54:
                    a9:8e:46:f8:e7:de:01:c9:f6:8c:8f:8d:65:8f:c8:
                    54:7b:c9:bc:a4:2a:8c:bb:f1:70:b3:96:7b:85:4a:
                    c7:7a:8a:c8:49:16:2f:25:df:5c:7a:b6:eb:bf:fa:
                    56:b5:d5:c0:4e:cb:c8:76:a1:35:73:9c:c2:45:3f:
                    8f:71:72:27:3f:06:41:0c:9c:f1:26:e6:9f:2c:50:
                    15:6d:35:47:af:7a:ef:6e:b3:75:20:a7:d5:fa:93:
                    b3:40:fe:38:96:4e:95:2f:45:8b:69:01:cb:50:1a:
                    b9:a3:da:af:3e:23:d5:d7:c6:46:ad:4b:9a:5c:23:
                    55:f0:23:09:81:06:9e:75:3d:6e:f6:e3:83:e3:57:
                    ce:f9:d2:70:8f:f1:14:6b:c5:30:8c:09:68:90:5f:
                    d4:48:cf:8c:0d:53:ec:70:de:34:c8:7a:00:a7:62:
                    7f:bd:2b:44:a1:83:09:e3:da:1a:44:79:d7:b6:24:
                    24:15:00:f7:d9:9d:9b:ca:20:30:9f:ac:5d:00:9c:
                    34:82:29:f0:2b:04:d4:6e:e5:1c:87:ca:98:aa:21:
                    fd:dd:be:57:d5:b1:f8:c3:84:2c:4b:1d:3b:72:61:
                    45:7f:af:2a:a9:21:6c:5e:b4:76:1e:de:ae:96:42:
                    b2:6e:9c:6d:f2:42:3b:66:8b:2a:6b:0e:6b:32:d9:
                    28:2d:ae:e8:0d:8e:3e:81:14:76:e2:c8:89:76:ee:
                    66:7d:c0:7e:0d:4e:bd:59:61:90:32:40:ed:8e:d8:
                    50:12:73:e7:5d:95:2a:6e:54:be:16:36:ee:d0:bc:
                    a3:ca:b0:74:3e:d9:b5:d6:5b:1a:f5:28:63:b2:74:
                    4c:11:5f:a1:74:82:aa:cc:23:d1:31:ee:9f:4d:41:
                    e0:de:30:21:24:3b:7b:22:11:47:8f:e4:67:d1:16:
                    1e:b6:6e:4a:ee:26:37:43:9e:1d:7e:27:ce:94:8b:
                    2f:a4:26:c6:be:66:99:42:b1:75:06:b0:c5:80:bd:
                    80:42:fc:f5:f8:68:a8:5c:42:c3:c7:4a:04:05:38:
                    36:de:98:31:d2:1c:9a:a4:b6:da:d3:d1:fe:18:ac:
                    95:45:49:91:d1:a3:a6:e2:79:fd:72:e1:09:8f:0d:
                    1e:71:09:69:56:b7:2f:ed:cf:d4:f6:90:e5:4b:12:
                    31:5f:95
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            X509v3 Key Usage: critical
                Certificate Sign, CRL Sign
            X509v3 Basic Constraints: critical
                CA:TRUE
            X509v3 Subject Key Identifier:
                05:69:C2:FC:BA:88:F0:E0:1C:F8:72:71:58:00:B8:9E:0A:80:71:A6
    Signature Algorithm: sha256WithRSAEncryption
    Signature Value:
        4f:a1:35:ec:30:d3:33:22:0b:23:3e:e6:20:8d:af:71:dd:92:
        4c:87:b9:48:c2:09:d1:cd:ee:7b:52:30:ea:7d:54:5b:66:4d:
        6e:6d:cd:1f:43:d2:13:2d:e4:0e:e0:13:0c:2e:5c:7e:ca:69:
        b1:16:b9:e1:01:39:55:2a:4b:e7:04:df:73:3a:1c:1a:18:36:
        55:f9:ed:26:0f:1d:78:1e:1c:d5:38:e7:7f:8e:fe:81:ba:9a:
        c3:80:cf:b3:b4:bb:08:54:5c:7d:14:55:46:42:a9:b0:32:ec:
        9b:51:cb:c0:23:c6:a5:0a:97:cb:a5:61:66:80:b4:72:c7:f8:
        91:2c:8a:ad:67:ba:df:34:c4:08:89:2f:11:0e:80:32:b8:c2:
        40:69:9a:d2:95:d5:18:cf:8d:e2:58:c1:b3:da:07:dd:ca:b8:
        d6:e1:1c:97:5a:7a:52:36:b8:aa:f9:6b:fb:b3:b3:f8:59:7c:
        d2:74:2a:0f:ad:5d:39:83:eb:d9:bf:c7:40:41:86:de:4b:1e:
        08:69:6e:54:3b:58:d6:35:96:08:1b:3c:ea:04:c6:af:24:7f:
        18:a0:6f:d3:a0:a1:49:ed:89:5a:51:8f:c0:8f:b5:41:33:ba:
        52:3c:1c:f2:8d:8e:ca:9b:71:41:2d:4f:d0:e3:f1:55:0c:ee:
        09:c4:15:2b:d2:9b:01:0d:ca:6d:51:a9:1f:fa:54:2f:e5:63:
        3f:34:c4:b0:39:f6:a6:f6:07:cd:9f:26:2a:34:48:db:e6:58:
        e2:2b:8e:eb:57:20:39:27:27:dc:4a:ea:57:36:4e:af:85:c2:
        27:88:be:2a:5f:33:fa:d2:d8:ed:1f:76:90:4f:f7:b8:78:89:
        25:81:d2:7a:43:ba:fe:8b:bf:74:97:92:31:f4:d4:43:fe:1a:
        b9:b0:07:32:6d:b1:d0:77:39:4b:5c:83:73:d6:34:11:d4:7f:
        a6:75:3b:81:9e:8d:be:73:01:45:ab:6d:50:d7:9f:1b:93:60:
        5c:ed:73:66:ff:d6:22:3e:fb:07:0d:79:c2:21:d1:84:63:c6:
        04:4f:d0:9d:12:0a:d9:a8:90:bc:d7:3b:a6:24:92:d2:46:75:
        24:23:78:81:36:b0:88:bd:3f:b5:49:76:c9:18:1d:ba:19:93:
        ec:32:48:7a:82:09:29:6d:fd:ff:4e:9f:96:df:b1:2b:78:08:
        a8:06:aa:3c:7d:49:84:43:e6:96:10:04:7c:46:6d:d5:86:97:
        c3:a2:14:b4:20:58:40:c2:2e:05:f2:55:c2:c9:ff:10:c9:83:
        b2:bf:df:41:6a:86:80:a5:be:2b:c0:4c:f3:ac:7d:9b:46:70:
        9f:83:ad:ae:4f:33:66:74


 and

         Issuer: C = US, O = Pearl TV, OU = Root CA-1, CN = ATSC3 Root-CA
        Validity
            Not Before: Sep  8 00:00:00 2021 GMT
            Not After : Sep  8 23:59:59 2030 GMT
        Subject: C = US, O = ATSC 3.0 Security Authority LLC, OU = Signing CA-DC1, CN = ATSC 3.0 Signing CA-2

 -----BEGIN CERTIFICATE-----
MIIFZjCCA06gAwIBAgIQfWz8GlbfqolzgvmmywRzPzANBgkqhkiG9w0BAQsFADBM
MQswCQYDVQQGEwJVUzERMA8GA1UEChMIUGVhcmwgVFYxEjAQBgNVBAsTCVJvb3Qg
Q0EtMTEWMBQGA1UEAxMNQVRTQzMgUm9vdC1DQTAgFw0xODA0MTkwMDAwMDBaGA8y
MDY4MDQxODIzNTk1OVowTDELMAkGA1UEBhMCVVMxETAPBgNVBAoTCFBlYXJsIFRW
MRIwEAYDVQQLEwlSb290IENBLTExFjAUBgNVBAMTDUFUU0MzIFJvb3QtQ0EwggIi
MA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDjBJrNXMq6TEy6dAaDi5Rr2nNS
ptvkboa2pVkoSwoCD6+OJ5UPMkclFsdIPfGUnEh/jYw/QwcLNrnwoipFV9+ZDbAj
z/xX0ZC6VKmORvjn3gHJ9oyPjWWPyFR7ybykKoy78XCzlnuFSsd6ishJFi8l31x6
tuu/+la11cBOy8h2oTVznMJFP49xcic/BkEMnPEm5p8sUBVtNUeveu9us3Ugp9X6
k7NA/jiWTpUvRYtpActQGrmj2q8+I9XXxkatS5pcI1XwIwmBBp51PW7244PjV875
0nCP8RRrxTCMCWiQX9RIz4wNU+xw3jTIegCnYn+9K0Shgwnj2hpEede2JCQVAPfZ
nZvKIDCfrF0AnDSCKfArBNRu5RyHypiqIf3dvlfVsfjDhCxLHTtyYUV/ryqpIWxe
tHYe3q6WQrJunG3yQjtmiyprDmsy2SgtrugNjj6BFHbiyIl27mZ9wH4NTr1ZYZAy
QO2O2FASc+ddlSpuVL4WNu7QvKPKsHQ+2bXWWxr1KGOydEwRX6F0gqrMI9Ex7p9N
QeDeMCEkO3siEUeP5GfRFh62bkruJjdDnh1+J86Uiy+kJsa+ZplCsXUGsMWAvYBC
/PX4aKhcQsPHSgQFODbemDHSHJqkttrT0f4YrJVFSZHRo6bief1y4QmPDR5xCWlW
ty/tz9T2kOVLEjFflQIDAQABo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/
BAUwAwEB/zAdBgNVHQ4EFgQUBWnC/LqI8OAc+HJxWAC4ngqAcaYwDQYJKoZIhvcN
AQELBQADggIBAE+hNeww0zMiCyM+5iCNr3HdkkyHuUjCCdHN7ntSMOp9VFtmTW5t
zR9D0hMt5A7gEwwuXH7KabEWueEBOVUqS+cE33M6HBoYNlX57SYPHXgeHNU453+O
/oG6msOAz7O0uwhUXH0UVUZCqbAy7JtRy8AjxqUKl8ulYWaAtHLH+JEsiq1nut80
xAiJLxEOgDK4wkBpmtKV1RjPjeJYwbPaB93KuNbhHJdaelI2uKr5a/uzs/hZfNJ0
Kg+tXTmD69m/x0BBht5LHghpblQ7WNY1lggbPOoExq8kfxigb9OgoUntiVpRj8CP
tUEzulI8HPKNjsqbcUEtT9Dj8VUM7gnEFSvSmwENym1RqR/6VC/lYz80xLA59qb2
B82fJio0SNvmWOIrjutXIDknJ9xK6lc2Tq+FwieIvipfM/rS2O0fdpBP97h4iSWB
0npDuv6Lv3SXkjH01EP+GrmwBzJtsdB3OUtcg3PWNBHUf6Z1O4Gejb5zAUWrbVDX
nxuTYFztc2b/1iI++wcNecIh0YRjxgRP0J0SCtmokLzXO6YkktJGdSQjeIE2sIi9
P7VJdskYHboZk+wySHqCCSlt/f9On5bfsSt4CKgGqjx9SYRD5pYQBHxGbdWGl8Oi
FLQgWEDCLgXyVcLJ/xDJg7K/30FqhoClvivATPOsfZtGcJ+Dra5PM2Z0
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIF7zCCA9egAwIBAgIQHYHAommgPZqvEoQoWT22uDANBgkqhkiG9w0BAQ0FADBM
MQswCQYDVQQGEwJVUzERMA8GA1UEChMIUGVhcmwgVFYxEjAQBgNVBAsTCVJvb3Qg
Q0EtMTEWMBQGA1UEAxMNQVRTQzMgUm9vdC1DQTAeFw0yMTA5MDgwMDAwMDBaFw0z
MDA5MDgyMzU5NTlaMHAxCzAJBgNVBAYTAlVTMSgwJgYDVQQKEx9BVFNDIDMuMCBT
ZWN1cml0eSBBdXRob3JpdHkgTExDMRcwFQYDVQQLEw5TaWduaW5nIENBLURDMTEe
MBwGA1UEAxMVQVRTQyAzLjAgU2lnbmluZyBDQS0yMIIBojANBgkqhkiG9w0BAQEF
AAOCAY8AMIIBigKCAYEAu7US2OxGzCtEMQZ7FR0fNv6d6hd+x3hV7Uu0H8wso+rX
br4+WhHmenMLLSS5VT0qchH9Bs8ntS15Ijxvgj8rj1ZzeHY7p9enLp9LnbmC4/Vt
iRM9W5w4oqCt7rHYG2soYweyoedjyhvXQSZzZPi3gSX5wgjwAINLbPc+1KwL61nP
Fqwuy8QeEitV9dhR+V5HS3kt7jkIru+aHFvmDhEKRxrZTZ1c1ayEC8AKnwABhTod
Rpvq14yNuAppStJ71DIIsqZk7H4YcUDSLhbgbGxN6UMWPb+U7zzTIGQBCp297vFC
OUHjvJvSM5QU3tfZM1QDNkftyIqDohKKBJKQGSODGPOHCkXPPisv9itKaM4gPMh/
GcZZRwiwt0i+nQwndmleViUGD9LMiDeNnm5z6qIO+YwaV4G+/HKUJ0SpVmA3vH/U
Pu0Zc0nvk8wvcVZtjdsu6m/ncSMQBjslZ6spgt01OQNGas4ODHyeqt46vdL2MxB8
bjv0zOa3DwOx2FvbJQnhAgMBAAGjggEnMIIBIzASBgNVHRMBAf8ECDAGAQH/AgEA
MDUGA1UdIAEB/wQrMCkwCwYJKwYBBAGDlGMCMAwGCisGAQQBg5RjAgEwDAYKKwYB
BAGDlGMCAjBMBgNVHR8ERTBDMEGgP6A9hjtodHRwOi8vcGtpLWNybC5zeW1hdXRo
LmNvbS9vZmZsaW5lY2EvUGVhcmxUVkFUU0MzUm9vdENBLmNybDAOBgNVHQ8BAf8E
BAMCAcYwHQYDVR0OBBYEFKDe/SSQWUgCIZcnf+GWO+KBaMa2MB8GA1UdIwQYMBaA
FAVpwvy6iPDgHPhycVgAuJ4KgHGmMDgGCCsGAQUFBwEBBCwwKjAoBggrBgEFBQcw
AYYcaHR0cDovL3BraS1vY3NwLmRpZ2ljZXJ0LmNvbTANBgkqhkiG9w0BAQ0FAAOC
AgEAEtmDPuS023t4GSaMMtMkPyh8IG5B2HOPtnwjXzf418ar2XCMcaRI7jGk12+V
POg5S6baCKin+QSgbGonU6mH9eLEhl2fCpRMnM6XhzdplCCj7xMZAHf5AhuCz+M5
tqaQelVv4WJrWHvMK1Jdw4q6GDtEKxF0cIYXyrinMqR65P2qvjEaJZUNdRSo/yux
u0UrkbNtuJ9TzQABnY5y7pTgcJOBbGes6bC/VrWurUXDFpeNfybm5PRZE9rlsIIe
T4J+3i7BGpUdnrPWQrk6NNa6M36XKxe1dh1HptyNXwOPamS5qpw7ewQARlP495uO
RvAM3p50TN2vtXKLgDQENi+Kmo/E+ORKaLJtgbet0jllhR9L7Gau8DdykvnTHkm2
/yxB0tDZTbrTRJciPjFHsRXmYQAyQYaod729shWngSgNE08n4RkiVSsSFbhrCXh+
ZrCF9wBpfCVoDaLemjriVy4SK7Hxmr39XxF66B9fBnGLXMvgyYRymNWPyys5j/wl
+iZt4KTm5W5iOhdIXaJxOO6b+EFeNZsDi8p52nvi5L5rtxw/1GAIiz39RTGd/w2t
hKQ+USVa7zY5sllzUBMjepeog/OOPwtbGccP38U6oW8vL00bQGbW5HuWP58qGcY9
Wk6I1mhinOmrpm1BsHqdYVFDCJZqQK4XMNukaxP6ey6xmJg=
-----END CERTIFICATE-----



*/


#ifndef ATSC3_A360_CERTIFICATES_H_
#define ATSC3_A360_CERTIFICATES_H_

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * Serial Number:
		7d:6c:fc:1a:56:df:aa:89:73:82:f9:a6:cb:04:73:3f
	Signature Algorithm: sha256WithRSAEncryption
	Issuer: C = US, O = Pearl TV, OU = Root CA-1, CN = ATSC3 Root-CA
	Validity
		Not Before: Apr 19 00:00:00 2018 GMT
		Not After : Apr 18 23:59:59 2068 GMT
	Subject: C = US, O = Pearl TV, OU = Root CA-1, CN = ATSC3 Root-CA
 ...
             X509v3 Subject Key Identifier:
                05:69:C2:FC:BA:88:F0:E0:1C:F8:72:71:58:00:B8:9E:0A:80:71:A6

 */
extern char* ATSC3_A360_CERTIFICATE_UTILS_BEGIN_CERTIFICATE;
extern char* ATSC3_A360_CERTIFICATE_UTILS_END_CERTIFICATE;

extern char* ATSC3_A360_CERTIFICATES_PEARL_A3SA_ROOT_CERT_SN_0569;
extern char* ATSC3_A360_CERTIFICATES_PEARL_A3SA_INTERMEDIATE_SIGNING_CA_2_SN_A0D3;
extern char* __OLD_ATSC3_CMS_UTILS_CDT_A3SA_ROOT_2020_CERT;

#if defined (__cplusplus)
}
#endif


#endif /* ATSC3_A360_CERTIFICATES_H_ */
