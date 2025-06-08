#!/bin/bash

ssh -i ~/.ssh/id_rsa gbabanis@scirouter.cslab.ece.ntua.gr -oHostKeyAlgorithms=+ssh-rsa -oPubkeyAcceptedKeyTypes=+ssh-rsa

