#!/bin/bash

scp -i ~/.ssh/id_rsa -r  -oHostKeyAlgorithms=+ssh-rsa -oPubkeyAcceptedKeyTypes=+ssh-rsa code gbabanis@scirouter.cslab.ece.ntua.gr:~ 

