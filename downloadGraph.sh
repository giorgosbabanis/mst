#!/bin/bash

scp -i ~/.ssh/id_rsa -oHostKeyAlgorithms=+ssh-rsa -oPubkeyAcceptedKeyTypes=+ssh-rsa gbabanis@scirouter.cslab.ece.ntua.gr:/s/graphs/$2 $1