package com.jupiter.boss.sdk.accesscontrol.authorization;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;

/**
 * This interface is based on the Oracle SSOToken Id.
 *
 */

public class SessionTokenIDImpl implements SessionTokenID {
    String sessionTokenID;

    SessionTokenIDImpl(String sessionTokenID) {
        this.sessionTokenID = sessionTokenID;
    }

    /**
     * Returns the encrypted session token string.
     * 
     * @return An encrypted session token string
     */
    public String getSessionTokenId() {
        return sessionTokenID;
    };
}
