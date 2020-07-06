package com.jupiter.mfdd.security.zk;

import java.io.IOException;

import org.apache.zookeeper.client.FourLetterWordMain;
import org.apache.zookeeper.common.X509Exception.SSLContextException;

public class FourLetterWordMainMfdd {
    public static void main(String[] args) throws IOException,
            SSLContextException {
        Credentials.setPropertiesClient();
        FourLetterWordMain.main(args);
    }
}
