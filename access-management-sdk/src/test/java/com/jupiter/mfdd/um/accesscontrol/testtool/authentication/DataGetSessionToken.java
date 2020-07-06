package com.jupiter.mfdd.um.accesscontrol.testtool.authentication;

import com.jupiter.boss.sdk.accesscontrol.SessionToken;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetSessionToken implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("getSessionToken(<sessionTokenId>):");

            SessionToken rv = new AccessControlFactoryImpl()
                    .getAuthenticationService().getSessionToken(sessionTokenId);

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}