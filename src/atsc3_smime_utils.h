//
//  atsc3_smime_utils.h
//  libatsc3
//
//  Created by Jason Justman on 11/17/20.
//  Copyright © 2020 Jason Justman. All rights reserved.
//

/*
 references:
	https://tools.ietf.org/html/rfc5751
 and
	a/360:2019
 
 smime header from A/360 for SLS validation, sample from enensys - 2020-11-17:
 

 ---sof---
 MIME-Version:1.0
 Content-Type: multipart/signed; protocol="application/pkcs7-signature";
  micalg=sha-256; boundary="AZo1dgOq(NcU,8(/(ZB-yZFrg4os'8ot(I,u2'r232KeC_gtYoAc8:Q74'llw5utTczJZU"

 --AZo1dgOq(NcU,8(/(ZB-yZFrg4os'8ot(I,u2'r232KeC_gtYoAc8:Q74'llw5utTczJZU
 Content-Type: multipart/related;
  type="application/mbms-envelope+xml";
  boundary="o'Upm1tAhkPGHkHy8cvdKYNxMC/p7,fj,1?MF6GPyrGJf1r-S7hdME-)NxtRjdK9I,xxG3"

 --o'Upm1tAhkPGHkHy8cvdKYNxMC/p7,fj,1?MF6GPyrGJf1r-S7hdME-)NxtRjdK9I,xxG3
 Content-Type: application/mbms-envelope+xml
 Content-Location: envelope.xml

 <?xml version="1.0" encoding="UTF-8"?>
....

 
 (hex dump):
 

 00000000  4d 49 4d 45 2d 56 65 72  73 69 6f 6e 3a 31 2e 30  |MIME-Version:1.0|
 00000010  0d 0a 43 6f 6e 74 65 6e  74 2d 54 79 70 65 3a 20  |..Content-Type: |
 00000020  6d 75 6c 74 69 70 61 72  74 2f 73 69 67 6e 65 64  |multipart/signed|
 00000030  3b 20 70 72 6f 74 6f 63  6f 6c 3d 22 61 70 70 6c  |; protocol="appl|
 00000040  69 63 61 74 69 6f 6e 2f  70 6b 63 73 37 2d 73 69  |ication/pkcs7-si|
 00000050  67 6e 61 74 75 72 65 22  3b 20 0d 0a 20 6d 69 63  |gnature"; .. mic|
 00000060  61 6c 67 3d 73 68 61 2d  32 35 36 3b 20 62 6f 75  |alg=sha-256; bou|
 00000070  6e 64 61 72 79 3d 22 41  5a 6f 31 64 67 4f 71 28  |ndary="AZo1dgOq(|
 00000080  4e 63 55 2c 38 28 2f 28  5a 42 2d 79 5a 46 72 67  |NcU,8(/(ZB-yZFrg|
 00000090  34 6f 73 27 38 6f 74 28  49 2c 75 32 27 72 32 33  |4os'8ot(I,u2'r23|
 000000a0  32 4b 65 43 5f 67 74 59  6f 41 63 38 3a 51 37 34  |2KeC_gtYoAc8:Q74|
 000000b0  27 6c 6c 77 35 75 74 54  63 7a 4a 5a 55 22 0d 0a  |'llw5utTczJZU"..|
 000000c0  0d 0a 2d 2d 41 5a 6f 31  64 67 4f 71 28 4e 63 55  |..--AZo1dgOq(NcU|
 000000d0  2c 38 28 2f 28 5a 42 2d  79 5a 46 72 67 34 6f 73  |,8(/(ZB-yZFrg4os|
 000000e0  27 38 6f 74 28 49 2c 75  32 27 72 32 33 32 4b 65  |'8ot(I,u2'r232Ke|
 000000f0  43 5f 67 74 59 6f 41 63  38 3a 51 37 34 27 6c 6c  |C_gtYoAc8:Q74'll|
 00000100  77 35 75 74 54 63 7a 4a  5a 55 0d 0a 43 6f 6e 74  |w5utTczJZU..Cont|
 00000110  65 6e 74 2d 54 79 70 65  3a 20 6d 75 6c 74 69 70  |ent-Type: multip|
 00000120  61 72 74 2f 72 65 6c 61  74 65 64 3b 0d 0a 20 74  |art/related;.. t|
 00000130  79 70 65 3d 22 61 70 70  6c 69 63 61 74 69 6f 6e  |ype="application|
 00000140  2f 6d 62 6d 73 2d 65 6e  76 65 6c 6f 70 65 2b 78  |/mbms-envelope+x|
 00000150  6d 6c 22 3b 0d 0a 20 62  6f 75 6e 64 61 72 79 3d  |ml";.. boundary=|
 00000160  22 6f 27 55 70 6d 31 74  41 68 6b 50 47 48 6b 48  |"o'Upm1tAhkPGHkH|
 00000170  79 38 63 76 64 4b 59 4e  78 4d 43 2f 70 37 2c 66  |y8cvdKYNxMC/p7,f|
 00000180  6a 2c 31 3f 4d 46 36 47  50 79 72 47 4a 66 31 72  |j,1?MF6GPyrGJf1r|
 00000190  2d 53 37 68 64 4d 45 2d  29 4e 78 74 52 6a 64 4b  |-S7hdME-)NxtRjdK|
 000001a0  39 49 2c 78 78 47 33 22  0d 0a 0d 0a 2d 2d 6f 27  |9I,xxG3"....--o'|
 000001b0  55 70 6d 31 74 41 68 6b  50 47 48 6b 48 79 38 63  |Upm1tAhkPGHkHy8c|
 000001c0  76 64 4b 59 4e 78 4d 43  2f 70 37 2c 66 6a 2c 31  |vdKYNxMC/p7,fj,1|
 000001d0  3f 4d 46 36 47 50 79 72  47 4a 66 31 72 2d 53 37  |?MF6GPyrGJf1r-S7|
 000001e0  68 64 4d 45 2d 29 4e 78  74 52 6a 64 4b 39 49 2c  |hdME-)NxtRjdK9I,|
 000001f0  78 78 47 33 0d 0a 43 6f  6e 74 65 6e 74 2d 54 79  |xxG3..Content-Ty|
 00000200  70 65 3a 20 61 70 70 6c  69 63 61 74 69 6f 6e 2f  |pe: application/|
 00000210  6d 62 6d 73 2d 65 6e 76  65 6c 6f 70 65 2b 78 6d  |mbms-envelope+xm|
 00000220  6c 0d 0a 43 6f 6e 74 65  6e 74 2d 4c 6f 63 61 74  |l..Content-Locat|
 00000230  69 6f 6e 3a 20 65 6e 76  65 6c 6f 70 65 2e 78 6d  |ion: envelope.xm|
 00000240  6c 0d 0a 0d 0a 3c 3f 78  6d 6c 20 76 65 72 73 69  |l....<?xml versi|
 
 
 
smime signature from A/360 for SLS validation, sample from enensys - 2020-11-17:

	</UserServiceDescription>
 </BundleDescriptionROUTE>

 --o'Upm1tAhkPGHkHy8cvdKYNxMC/p7,fj,1?MF6GPyrGJf1r-S7hdME-)NxtRjdK9I,xxG3--

 --AZo1dgOq(NcU,8(/(ZB-yZFrg4os'8ot(I,u2'r232KeC_gtYoAc8:Q74'llw5utTczJZU
 Content-Type: application/pkcs7-signature; name=bcsig.p7s; mime-type=signed-data
 Content-Transfer-Encoding: base64;
 Content-Disposition: attachment; filemane="bcsig7.p7s"

 MIICXgYJKoZIhvcNAQcCoIICTzCCAksCAQMxDTALBglghkgBZQMEAgEwCwYJKoZI
 hvcNAQcBMYICKDCCAiQCAQOAFK3ctxQf/TQvkxUJ2eZXvYL44UtzMAsGCWCGSAFl
 AwQCAaBpMBgGCSqGSIb3DQEJAzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkFMQ8X
 DTIwMTEwNTE5NTkzNFowLwYJKoZIhvcNAQkEMSIEICipdZWY75JYSRuC9gjVh/gh
 pLQsT0HFRhjnYDjJRiYYMA0GCSqGSIb3DQEBAQUABIIBgB1BVGFLoSXXn8Bvqcrd
 E7qR0V6hthwenO0VPRNNh2XoRdGaYAMnI7gSDozLle7Dv0DqRGIXfBWS1VF+bAAe
 skfJSJNxtfmQnHypn2e/zKBX51HMnPl4P00GRhLetcElEyXSXCtyTFQa1X8sOYi/
 A3JXrabLn+4/olxpMDsP7yhABob1NBGwz9/i+VuuAK1z3woRlUySkI6zvhTsM/pr
 q6b/HwtMEI56XWHCpbV3z8AlZ3z5pohpYY1ux/0tV2bbpjHmPFT7LISjLhwxmmOa
 w0PrPacTglwLK/+QkFVQXOoGEd3cs/Zu38EoHhxaIepuUbUmNrLbaN6Mb/3kELGB
 UIkX6J0UKyplK3B7FEh1u5Wipj2fHriS0vQIrrev4Wblp52E81KAQq+fT8o/xbrj
 WQoxYtIxR8lNhRxU+GfA7pe8V9Q97uxfCxRXTWUo4czNWYgzziitleg5lEp0Dc+u
 Vm8ShvHMbEjJ7Yh00/NxTka9wXDh11TnzkoGyT1o6P05rQ==

 --AZo1dgOq(NcU,8(/(ZB-yZFrg4os'8ot(I,u2'r232KeC_gtYoAc8:Q74'llw5utTczJZU--

 
 (hexdump):
 
 00001570  69 6f 6e 3e 0d 0a 3c 2f  42 75 6e 64 6c 65 44 65  |ion>..</BundleDe|
 00001580  73 63 72 69 70 74 69 6f  6e 52 4f 55 54 45 3e 0d  |scriptionROUTE>.|
 00001590  0a 0d 0a 2d 2d 6f 27 55  70 6d 31 74 41 68 6b 50  |...--o'Upm1tAhkP|
 000015a0  47 48 6b 48 79 38 63 76  64 4b 59 4e 78 4d 43 2f  |GHkHy8cvdKYNxMC/|
 000015b0  70 37 2c 66 6a 2c 31 3f  4d 46 36 47 50 79 72 47  |p7,fj,1?MF6GPyrG|
 000015c0  4a 66 31 72 2d 53 37 68  64 4d 45 2d 29 4e 78 74  |Jf1r-S7hdME-)Nxt|
 000015d0  52 6a 64 4b 39 49 2c 78  78 47 33 2d 2d 0d 0a 0d  |RjdK9I,xxG3--...|
 000015e0  0a 2d 2d 41 5a 6f 31 64  67 4f 71 28 4e 63 55 2c  |.--AZo1dgOq(NcU,|
 000015f0  38 28 2f 28 5a 42 2d 79  5a 46 72 67 34 6f 73 27  |8(/(ZB-yZFrg4os'|
 00001600  38 6f 74 28 49 2c 75 32  27 72 32 33 32 4b 65 43  |8ot(I,u2'r232KeC|
 00001610  5f 67 74 59 6f 41 63 38  3a 51 37 34 27 6c 6c 77  |_gtYoAc8:Q74'llw|
 00001620  35 75 74 54 63 7a 4a 5a  55 0d 0a 43 6f 6e 74 65  |5utTczJZU..Conte|
 00001630  6e 74 2d 54 79 70 65 3a  20 61 70 70 6c 69 63 61  |nt-Type: applica|
 00001640  74 69 6f 6e 2f 70 6b 63  73 37 2d 73 69 67 6e 61  |tion/pkcs7-signa|
 00001650  74 75 72 65 3b 20 6e 61  6d 65 3d 62 63 73 69 67  |ture; name=bcsig|
 00001660  2e 70 37 73 3b 20 6d 69  6d 65 2d 74 79 70 65 3d  |.p7s; mime-type=|
 00001670  73 69 67 6e 65 64 2d 64  61 74 61 0d 0a 43 6f 6e  |signed-data..Con|
 00001680  74 65 6e 74 2d 54 72 61  6e 73 66 65 72 2d 45 6e  |tent-Transfer-En|
 00001690  63 6f 64 69 6e 67 3a 20  62 61 73 65 36 34 3b 0d  |coding: base64;.|
 000016a0  0a 43 6f 6e 74 65 6e 74  2d 44 69 73 70 6f 73 69  |.Content-Disposi|
 000016b0  74 69 6f 6e 3a 20 61 74  74 61 63 68 6d 65 6e 74  |tion: attachment|
 000016c0  3b 20 66 69 6c 65 6d 61  6e 65 3d 22 62 63 73 69  |; filemane="bcsi|
 000016d0  67 37 2e 70 37 73 22 0d  0a 0d 0a 4d 49 49 43 58  |g7.p7s"....MIICX|
 000016e0  67 59 4a 4b 6f 5a 49 68  76 63 4e 41 51 63 43 6f  |gYJKoZIhvcNAQcCo|
 000016f0  49 49 43 54 7a 43 43 41  6b 73 43 41 51 4d 78 44  |IICTzCCAksCAQMxD|
 00001700  54 41 4c 42 67 6c 67 68  6b 67 42 5a 51 4d 45 41  |TALBglghkgBZQMEA|
 00001710  67 45 77 43 77 59 4a 4b  6f 5a 49 0a 68 76 63 4e  |gEwCwYJKoZI.hvcN|
 00001720  41 51 63 42 4d 59 49 43  4b 44 43 43 41 69 51 43  |AQcBMYICKDCCAiQC|
 00001730  41 51 4f 41 46 4b 33 63  74 78 51 66 2f 54 51 76  |AQOAFK3ctxQf/TQv|
 00001740  6b 78 55 4a 32 65 5a 58  76 59 4c 34 34 55 74 7a  |kxUJ2eZXvYL44Utz|
 00001750  4d 41 73 47 43 57 43 47  53 41 46 6c 0a 41 77 51  |MAsGCWCGSAFl.AwQ|
 00001760  43 41 61 42 70 4d 42 67  47 43 53 71 47 53 49 62  |CAaBpMBgGCSqGSIb|
 00001770  33 44 51 45 4a 41 7a 45  4c 42 67 6b 71 68 6b 69  |3DQEJAzELBgkqhki|
 00001780  47 39 77 30 42 42 77 45  77 48 41 59 4a 4b 6f 5a  |G9w0BBwEwHAYJKoZ|
 00001790  49 68 76 63 4e 41 51 6b  46 4d 51 38 58 0a 44 54  |IhvcNAQkFMQ8X.DT|
 000017a0  49 77 4d 54 45 77 4e 54  45 35 4e 54 6b 7a 4e 46  |IwMTEwNTE5NTkzNF|
 000017b0  6f 77 4c 77 59 4a 4b 6f  5a 49 68 76 63 4e 41 51  |owLwYJKoZIhvcNAQ|
 000017c0  6b 45 4d 53 49 45 49 43  69 70 64 5a 57 59 37 35  |kEMSIEICipdZWY75|
 000017d0  4a 59 53 52 75 43 39 67  6a 56 68 2f 67 68 0a 70  |JYSRuC9gjVh/gh.p|
 000017e0  4c 51 73 54 30 48 46 52  68 6a 6e 59 44 6a 4a 52  |LQsT0HFRhjnYDjJR|
 000017f0  69 59 59 4d 41 30 47 43  53 71 47 53 49 62 33 44  |iYYMA0GCSqGSIb3D|
 00001800  51 45 42 41 51 55 41 42  49 49 42 67 42 31 42 56  |QEBAQUABIIBgB1BV|
 00001810  47 46 4c 6f 53 58 58 6e  38 42 76 71 63 72 64 0a  |GFLoSXXn8Bvqcrd.|
 00001820  45 37 71 52 30 56 36 68  74 68 77 65 6e 4f 30 56  |E7qR0V6hthwenO0V|
 00001830  50 52 4e 4e 68 32 58 6f  52 64 47 61 59 41 4d 6e  |PRNNh2XoRdGaYAMn|
 00001840  49 37 67 53 44 6f 7a 4c  6c 65 37 44 76 30 44 71  |I7gSDozLle7Dv0Dq|
 00001850  52 47 49 58 66 42 57 53  31 56 46 2b 62 41 41 65  |RGIXfBWS1VF+bAAe|
 00001860  0a 73 6b 66 4a 53 4a 4e  78 74 66 6d 51 6e 48 79  |.skfJSJNxtfmQnHy|
 00001870  70 6e 32 65 2f 7a 4b 42  58 35 31 48 4d 6e 50 6c  |pn2e/zKBX51HMnPl|
 00001880  34 50 30 30 47 52 68 4c  65 74 63 45 6c 45 79 58  |4P00GRhLetcElEyX|
 00001890  53 58 43 74 79 54 46 51  61 31 58 38 73 4f 59 69  |SXCtyTFQa1X8sOYi|
 000018a0  2f 0a 41 33 4a 58 72 61  62 4c 6e 2b 34 2f 6f 6c  |/.A3JXrabLn+4/ol|
 000018b0  78 70 4d 44 73 50 37 79  68 41 42 6f 62 31 4e 42  |xpMDsP7yhABob1NB|
 000018c0  47 77 7a 39 2f 69 2b 56  75 75 41 4b 31 7a 33 77  |Gwz9/i+VuuAK1z3w|
 000018d0  6f 52 6c 55 79 53 6b 49  36 7a 76 68 54 73 4d 2f  |oRlUySkI6zvhTsM/|
 000018e0  70 72 0a 71 36 62 2f 48  77 74 4d 45 49 35 36 58  |pr.q6b/HwtMEI56X|
 000018f0  57 48 43 70 62 56 33 7a  38 41 6c 5a 33 7a 35 70  |WHCpbV3z8AlZ3z5p|
 00001900  6f 68 70 59 59 31 75 78  2f 30 74 56 32 62 62 70  |ohpYY1ux/0tV2bbp|
 00001910  6a 48 6d 50 46 54 37 4c  49 53 6a 4c 68 77 78 6d  |jHmPFT7LISjLhwxm|
 00001920  6d 4f 61 0a 77 30 50 72  50 61 63 54 67 6c 77 4c  |mOa.w0PrPacTglwL|
 00001930  4b 2f 2b 51 6b 46 56 51  58 4f 6f 47 45 64 33 63  |K/+QkFVQXOoGEd3c|
 00001940  73 2f 5a 75 33 38 45 6f  48 68 78 61 49 65 70 75  |s/Zu38EoHhxaIepu|
 00001950  55 62 55 6d 4e 72 4c 62  61 4e 36 4d 62 2f 33 6b  |UbUmNrLbaN6Mb/3k|
 00001960  45 4c 47 42 0a 55 49 6b  58 36 4a 30 55 4b 79 70  |ELGB.UIkX6J0UKyp|
 00001970  6c 4b 33 42 37 46 45 68  31 75 35 57 69 70 6a 32  |lK3B7FEh1u5Wipj2|
 00001980  66 48 72 69 53 30 76 51  49 72 72 65 76 34 57 62  |fHriS0vQIrrev4Wb|
 00001990  6c 70 35 32 45 38 31 4b  41 51 71 2b 66 54 38 6f  |lp52E81KAQq+fT8o|
 000019a0  2f 78 62 72 6a 0a 57 51  6f 78 59 74 49 78 52 38  |/xbrj.WQoxYtIxR8|
 000019b0  6c 4e 68 52 78 55 2b 47  66 41 37 70 65 38 56 39  |lNhRxU+GfA7pe8V9|
 000019c0  51 39 37 75 78 66 43 78  52 58 54 57 55 6f 34 63  |Q97uxfCxRXTWUo4c|
 000019d0  7a 4e 57 59 67 7a 7a 69  69 74 6c 65 67 35 6c 45  |zNWYgzziitleg5lE|
 000019e0  70 30 44 63 2b 75 0a 56  6d 38 53 68 76 48 4d 62  |p0Dc+u.Vm8ShvHMb|
 000019f0  45 6a 4a 37 59 68 30 30  2f 4e 78 54 6b 61 39 77  |EjJ7Yh00/NxTka9w|
 00001a00  58 44 68 31 31 54 6e 7a  6b 6f 47 79 54 31 6f 36  |XDh11TnzkoGyT1o6|
 00001a10  50 30 35 72 51 3d 3d 0a  0d 0a 2d 2d 41 5a 6f 31  |P05rQ==...--AZo1|
 00001a20  64 67 4f 71 28 4e 63 55  2c 38 28 2f 28 5a 42 2d  |dgOq(NcU,8(/(ZB-|
 00001a30  79 5a 46 72 67 34 6f 73  27 38 6f 74 28 49 2c 75  |yZFrg4os'8ot(I,u|
 00001a40  32 27 72 32 33 32 4b 65  43 5f 67 74 59 6f 41 63  |2'r232KeC_gtYoAc|
 00001a50  38 3a 51 37 34 27 6c 6c  77 35 75 74 54 63 7a 4a  |8:Q74'llw5utTczJ|
 00001a60  5a 55 2d 2d 0d 0a                                 |ZU--..|

 
 
 
 */

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"


#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/err.h>

#ifndef ATSC3_SMIME_UTILS_H_
#define ATSC3_SMIME_UTILS_H_


#if defined (__cplusplus)
extern "C" {
#endif

typedef struct atsc3_smime_entity {
	char*	mime_version;
	char*	content_type;
	char*	protocol;
	char*	micalg;
	char*	boundary;
	
	char*		raw_smime_payload_filename;
	block_t*	raw_smime_payload;
	block_t*	extracted_mime_entity;
	block_t*	extracted_pkcs7_signature;
	
} atsc3_smime_entity_t;


typedef struct atsc3_smime_validation_context {
	atsc3_smime_entity_t* atsc3_smime_entity;
	//other CDT information here...
	
	bool cms_signature_valid;
		
} atsc3_smime_validation_context_t;

atsc3_smime_entity_t* atsc3_smime_entity_new();

atsc3_smime_entity_t* atsc3_smime_entity_new_parse_from_file(const char* filename);

void atsc3_smime_entity_free(atsc3_smime_entity_t** atsc3_smime_entity_p);

atsc3_smime_validation_context_t* atsc3_smime_validation_context_new(atsc3_smime_entity_t* atsc3_smime_entity);
void atsc3_smime_validation_context_free(atsc3_smime_validation_context_t** atsc3_smime_validation_context_p);

atsc3_smime_validation_context_t* atsc3_smime_validate_from_context(atsc3_smime_validation_context_t* atsc3_smime_validation_context);

#if defined (__cplusplus)
}
#endif



#define _ATSC3_SMIME_UTILS_ERROR(...)  __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_SMIME_UTILS_WARN(...)   __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_SMIME_UTILS_INFO(...)   if(_ATSC3_SMIME_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _ATSC3_SMIME_UTILS_DEBUG(...)  if(_ATSC3_SMIME_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_SMIME_UTILS_TRACE(...)  if(_ATSC3_SMIME_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif /* ATSC3_SMIME_UTILS_H_ */
