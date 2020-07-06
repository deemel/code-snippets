package com.jupiter.boss.sdk.accesscontrol.authentication;

//import com.jupiter.boss.sdk.accesscontrol.AuthXResponseCode;
import javax.ws.rs.core.Response.Status;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;

/**
 * This interface represents the authentication response.<br>
 * This contains the authentication result, SSOToken object when the
 * authentication is successful,<br>
 * and other authentication response attributes. The fields are set during the
 * class instance construction to make the object immutable. The fields should
 * be defined as final and private. <br>
 * 
 * @version 1.0.0
 */

public interface AuthenticationResponse {
    /**
     * Get a session token id.
     * 
     * @return the Session Token Id; when the user authentication is successful,
     *         it returns a user session id, otherwise, the returned session
     *         token id will be null.<br>
     */
    public SessionTokenID getSessionTokenID();

    /**
     * Get a response code. The response code derive the response reason and
     * message that supports internationalization.
     * 
     * @return the response code <br>
     */
    public Status getResponseCode();

    /**
     * Get a response URL.
     * 
     * @return the response url String
     */
    public String getResponseUrl();
}
