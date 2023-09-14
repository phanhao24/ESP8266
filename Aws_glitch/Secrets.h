#include <pgmspace.h>
 
#define SECRET
     
 
#define THINGNAME "station03"
 
int8_t TIME_ZONE = 7; //NYC(VN): +7 UTC
 
const char AWS_IOT_ENDPOINT[] = "al4a2qdhosxnv-ats.iot.ap-east-1.amazonaws.com";  
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAPJCCZLne2ic1joGhGsEYBZhJvSdMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzA3MDMwNTI3
MjJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDL4YeCghHZ2xO/CsFd
/dIYLoJF/94kNiIjJixPSnDYtlz+Gc18T2SEkYeK6R703DEw652sAviA3XrsHY28
GUI4249WOc4X6VxFP6xVuwI0BwwC1EGJyEl/sIlG1+zDIa2ZdCDP62K5UCSiU3Sl
LvoLWO2g6h/HJndwPMfhu2jt4O0oyhvJOLVWuH5SPO9KoXnolqHoZMTW9jRjIcqE
TgNTwt63ITt5Eg7wk/4KK0mocpwYqTkVj+u/Yg+DP8i0Q+5DqaHHCJ6Au9luS6A8
qKvn0CzbSvotABIANVoCgypcCf1vVs0+oBOJmowZXDEqSoG4MEdC7cP/5+p0qW2v
ceahAgMBAAGjYDBeMB8GA1UdIwQYMBaAFHwtzENjpkB20/UQ3blPP2u3+BKHMB0G
A1UdDgQWBBSBSZ5XVrIVZiXSZIqKxL2sYgr5fDAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAtNsZCOM2fHB0r0nmKQhnQ2F8
ts07Sy9MjykALJzyw5y86PaiNA7nmaxFmNsBjXIozdu/5Uy1tlFhthhZVdErTuow
9SBp7QZtYrce1BPMKyevO/0Y5ym3bgVoKgtssbuwPXAEpFCtdQ0+/9HbBAcoAjgz
4WmlVbJHyAwIvqehy3Vc0WJNqtT/no21rBTQhBwl1FrhUiiOA632daVDKnwL+p0J
qwRjJ94KxhpBMEAlNMtZWPDhjgAnzJUFEM0UI9cemq3Oc4wUOEh3aabWF3SNdnCe
jK5G0ISL5a08JJaQrk3llBjUXzZRL06YFBv2P8Vdkb2mMonEg/PdqTQo41Y+vw==
-----END CERTIFICATE-----
 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAy+GHgoIR2dsTvwrBXf3SGC6CRf/eJDYiIyYsT0pw2LZc/hnN
fE9khJGHiuke9NwxMOudrAL4gN167B2NvBlCONuPVjnOF+lcRT+sVbsCNAcMAtRB
ichJf7CJRtfswyGtmXQgz+tiuVAkolN0pS76C1jtoOofxyZ3cDzH4bto7eDtKMob
yTi1Vrh+UjzvSqF56Jah6GTE1vY0YyHKhE4DU8LetyE7eRIO8JP+CitJqHKcGKk5
FY/rv2IPgz/ItEPuQ6mhxwiegLvZbkugPKir59As20r6LQASADVaAoMqXAn9b1bN
PqATiZqMGVwxKkqBuDBHQu3D/+fqdKltr3HmoQIDAQABAoIBAQCNC+JupENrPweD
JBKwTre5KPvRJ3lr/dNSE8eVtz8ofckJtZoWBOraV7+4lxxmc6rIWEiFRqzaod63
WJ7EYYkRwruAAI+WtUezVKCJ0s1sZu+lSWhpxJkscFoYG6tdN4Bc6B0HGqX64/nk
Hu5UlJJBpW0H5NDFOtRoXnQ9sySdY8EjM59UPj3UPG3NPIPK4J8EsEQT3qvUHdVB
1nviSTXtBghFgNRYXMPYawZz5fl1Z5JE607hmsBgLWWBVE/ay9FL+HLYrUnzGDlB
emJBAcYVLmrjE7z+0cetGnnDuIkJt+c4a7TXJcrY5JSrNOQcNanwyv9sNCJoV7XZ
gOYaKEEdAoGBAOkhJE6UXJmPKPkrrkYG52y4BGKc7vNEcNZRxcjX37DCWeAmpZ6C
/8UHiPJYZNf25EHY8XrOGwmPr/ZUeaCe0UBo0DPtMN2KqG0TuMaOGZ+BYcKw/F27
FG8u9kDRyKY/zjmz3qejTmlGmynO/Ab+Px21Sca8v7l1YY/mNLvAbObLAoGBAN/h
1dw/xtDSTd6okvU5BNlu+pOD1ynZP8S1hd7DcegiB8j9H6+K0vDsOnnMeuHyU0EG
HELCj3jqhgTit/0khRpHAEKWf3sUbgAE12jsB80DsNIyT2DsdAbJZa5AwcL+BprB
QGkH2A9CgnSI/CHU8fP32dA/37vuDWtHnjzMvQ7DAoGBAJ6R9WEhlu64KpV5OZvW
HFmdvRXaSsnbFqkgi9h1xWg0ee92OhbcYd6Bpou4xjcDbvS+/FmckVVqj7SMdwJi
pFAcOunm/uLh6nfu/zLG+vS8i2NjAPU8R8Tse4FRcBpssfgbuWnrMW14IhhI+rJD
A2lQozLztbEO/2JioiHG3AwLAoGADWQktpfZ/IOXxMmwgJp4f8jzk/8+z3H/LnTt
HPXzyj6+2HbLfmR/WYnHtaFVeE5WTH1hE10v17uvaHtnJe0ZSQkWKuV1mpcOPVIM
0GnNzaV63hfER7fc+MrzGgkljdRD8N2+TIn7km4X/4ByFFP4ZO9eR+aKK2OByfju
rhRQ+qECgYEAsx0iI5JXB6b40HAbqBXF2SYRBfFS80tpVZ2O4AbEKHz9AQyR03bm
BC7g9mS74eZIw66rl99w9dJOrgk53rve2Nbvc7M7vOoXQf6GATJ7VdPGlBQRFAXq
JLiJq1wQtOvApmMhMtKzBNUM8EKcm0xbbrM0+nvAliRBN/wR49rVMjQ=
-----END RSA PRIVATE KEY-----
 
 
)KEY";

