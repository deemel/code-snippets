package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import java.net.InetAddress;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetIPAddress implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("getIPAddress():");

            InetAddress rv = new AccessControlFactoryImpl()
                    .getAuthenticationService().getSessionToken(sessionTokenId)
                    .getIPAddress();

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
