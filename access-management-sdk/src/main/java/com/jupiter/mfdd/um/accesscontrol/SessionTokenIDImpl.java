package com.jupiter.mfdd.um.accesscontrol;

import org.apache.commons.lang3.StringUtils;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;
import com.jupiter.mfdd.um.accesscontrol.support.Realm;

/**
 * Implementation of session token ID interface to support OpenAM RESTful
 * services.
 */
public class SessionTokenIDImpl implements SessionTokenID {
    private String sessionTokenIdString;
    private Realm realm;
    private String userId;
    private String password;

    /**
     * Constructs session token ID with specified parameters.
     * 
     * @param sessionTokenIdString
     *            session token ID string
     * @param realm
     *            realm
     * @param userId
     *            user ID
     * @param password
     *            password
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public SessionTokenIDImpl(String sessionTokenIdString, Realm realm,
            String userId, String password) throws AuthXException {
        if (StringUtils.isEmpty(sessionTokenIdString))
            throw new AuthXException("Invalid session token ID string <"
                    + sessionTokenIdString + ">.");

        OpenAmRestUtil.validateUserId(userId);
        OpenAmRestUtil.validatePassword(password);

        this.sessionTokenIdString = sessionTokenIdString;
        this.realm = realm;
        this.userId = userId;
        this.password = password;
    }

    /**
     * Returns session token ID string.
     * 
     * @return session token ID string
     */
    @Override
    public String getSessionTokenId() {
        return sessionTokenIdString;
    }

    /**
     * Returns realm.
     * 
     * @return realm
     */
    public Realm getRealm() {
        return realm;
    }

    /**
     * Returns user ID.
     * 
     * @return user ID
     */
    public String getUserId() {
        return userId;
    }

    /**
     * Returns password.
     * 
     * @return password
     */
    public String getPassword() {
        return password;
    }

    /**
     * Returns session token ID string.
     * 
     * @return session token ID string
     */
    public String toString() {
        return getSessionTokenId();
    }

    /**
     * Returns <code>true</code> if current object's session token ID string is
     * equal to specified object's session token ID string.
     * 
     * @param object
     *            object for comparison
     * @return <code>true</code> if current object's session token ID string is
     *         equal to specified object's session token ID string
     */
    public boolean equals(Object object) {
        if (object == this)
            return true;

        if (!(object instanceof SessionTokenID))
            return false;

        return ((SessionTokenID) object).getSessionTokenId().equals(
                sessionTokenIdString);
    }
}
