package com.jupiter.mfdd.um.accesscontrol.testtool.authentication;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataRefreshSession implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("refreshSession(<sessionTokenId>):");

            boolean rv = new AccessControlFactoryImpl()
                    .getAuthenticationService().refreshSession(sessionTokenId);

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
