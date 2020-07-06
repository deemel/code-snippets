package com.jupiter.boss.sdk.accesscontrol.authentication;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionToken;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;

/**
 * This interface represents authentication services between the application and
 * the authentication server.<br>
 * This interface allows the application to check authentication against the
 * authentication server.<br>
 * Note that this does not handle the end-user login process; that can be
 * supported by JAAS or interceptor by following<br>
 * authentication externalization principals. So, the applications do not need
 * to embed their own authentication modules.<br>
 * These authenticate methods are to support stand-alone authentication
 * processing, otherwise, the process should start<br>
 * from the getSessionToken(String ssoTokenId), which is based on the existing
 * authenticated session.<br>
 * <br>
 * | application -- Authenticationservice -- Authentication Server | <br>
 * 
 * @version 1.0.0
 */
public interface AuthenticationService {
    /**
     * It gets an authentication response based on the orgName/realm, userId and
     * password.<br>
     * Depending on the authentication server multi-realm/tenant capability,
     * this method supports authentication processes in the multi-tenant
     * environment.<br>
     * It is used for Zero page login, and it works for name/password
     * authentication.<br>
     * Multi-factor authentication should be supported by utilizing interceptors
     * with authentication server(s)
     * 
     * @param realm
     *            The name of the user's organization; it often represents a
     *            realm that the user belongs. If this is empty, this method
     *            assume the orgName is the top level ("/").
     * @param userId
     *            The User Id
     * @param password
     *            The password
     * @throws AuthXException
     *             throws AuthXException when the authenticate method has a
     *             runtime exception.
     * @return AuthenticationResponse - if success the result is true; else
     *         false with SSOToken is null
     */
    public AuthenticationResponse authenticate(String realm, String userId,
            String password) throws AuthXException;

    /**
     * This method is used to authenticate a user using x509-based certificate
     * including SAML.
     * 
     * @param realm
     *            realm name where the certificate is stored in PAP
     * @param x509CertificateString
     *            A String represents x509 Certificate
     * 
     * @return AuthenticationResponse - if success the result is true; else
     *         false with SSOToken is null
     *
     */
    // public AuthenticationResponse authenticateWithCertifcate(String realm,
    // String x509CertificateString) throws AuthXException;

    /**
     * Get an existing session token object based on the SSOTokenId.<br>
     *
     * @param sessionTokenId
     *            token id String
     * @throws AuthXException
     *             throws AuthXException when the getSessionToken has a runtime
     *             exception.
     * @return SessionToken - returns a session token
     */
    public SessionToken getSessionToken(SessionTokenID sessionTokenId)
            throws AuthXException;

    /**
     * Refresh the Session corresponding to the token id. The session attributes
     * such as idle-time will be reset.<br>
     * 
     * @param sessionTokenId
     *            token id String
     * @throws AuthXException
     *             - if the session reached its maximum session time, or the
     *             session was destroyed,<br>
     *             or there was an error while refreshing the session.
     * @return boolean - true or false
     */
    public boolean refreshSession(SessionTokenID sessionTokenId)
            throws AuthXException;

    /**
     * Destroy an existing SSOToken session for a given SSOToken Id.<br>
     * This method gets the Authentication Server SSOToken object and maps into
     * the SSOToken object.
     * 
     * @param sessionTokenId
     *            SSOToken Id String
     * @throws AuthXException
     *             - if an error occurred during destroying
     * @return boolean - true or false
     */
    public boolean destroySession(SessionTokenID sessionTokenId)
            throws AuthXException;

    /**
     * Validate the given ssoTokenId.<br>
     * 
     * @param sessionTokenId
     *            SessionTokenID
     * @throws AuthXException
     *             - if an error occurred
     * @return boolean - true or false
     */
    public boolean isValidSession(SessionTokenID sessionTokenId)
            throws AuthXException;
}
