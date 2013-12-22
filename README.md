# cs240-tweeter

Twitter client for Purdue CS240 class based on O2 project.

## Requirements:

QtSDK 4.8 or Qt <5

## Options:

  -o            Link with Twitter OAuth service, i.e get access tokens

  -x            Link with Twitter XAuth service, i.e get access tokens using the XAuth protocol

  -u <username> Twitter username to be used while using XAuth (-x option)

  -p <password> Twitter password to be used while using XAuth (-x option)

  -m            Status update message, enclosed in double quotes

  -f <filename> The file that contains status update message, used when -m is not specified

  -r            Read message from standard in

## Precedence:
    
    "-m" > "-f" > "-r"
