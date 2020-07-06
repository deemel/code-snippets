/**
 * This interface is inspired from the Sun's SSOToken, and will hold user session information.
 */

package com.jupiter.boss.sdk.accesscontrol;

/**
 * This interface is inspired from the Sun's SSOToken, and will hold user
 * session information.<br>
 * The <code>SessionToken</code> class represents a single user session token.
 * It contains session token-related information such as authentication method
 * used for authentication, authentication level of the authentication method,
 * host name of the client that sent the request (browser). It also contains
 * session-related information such as maximum session time, maximum session
 * idle time and session idle time.
 *
 * @version 1.0.0
 */
public interface SessionToken {

    /**
     * Returns the value of the property "Principal" set to the session token.
     * 
     * @return The principal name
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the principal
     */
    public java.security.Principal getPrincipal() throws AuthXException;

    /**
     * Returns the authentication method used for authentication.
     * 
     * @return The authentication method
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the authentication method
     */
    public java.lang.String getAuthType() throws AuthXException;

    /**
     * Returns the authentication level of the authentication method used for
     * authentication.
     * 
     * @return The authentication level
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the authentication level
     */
    public int getAuthLevel() throws AuthXException;

    /**
     * Returns the IP Address of the client (browser) that sent the request.
     * 
     * @return The IP Address of the client
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the IP Address of the client
     */
    public java.net.InetAddress getIPAddress() throws AuthXException;

    /**
     * Returns the host name of the client (browser) that sent the request.
     * 
     * @return The host name of the client
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the host name of the client
     */
    public java.lang.String getHostName() throws AuthXException;

    /**
     * Returns the time left in seconds on the session based on max session
     * time.
     * 
     * @return The time left in seconds on the session.
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the maximum session time.
     */
    public long getTimeLeft() throws AuthXException;

    /**
     * Returns the maximum session time in minutes.
     * 
     * @return The maximum session time in minutes
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the maximum session time
     */
    public long getMaxSessionTime() throws AuthXException;

    /**
     * Returns the session idle time in seconds.
     * 
     * @return The session idle time in seconds
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the session idle time
     */
    public long getIdleTime() throws AuthXException;

    /**
     * Returns the maximum session idle time in minutes.
     * 
     * @return The maximum session idle time in minutes
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the maximum idle time
     */
    public long getMaxIdleTime() throws AuthXException;

    /**
     * Returns session token ID object.
     * 
     * @return session token ID.
     */
    public SessionTokenID getSessionTokenID();

    /**
     * Sets a property for this token.
     * 
     * @param name
     *            The property name.
     * @param value
     *            The property value.
     * @exception AuthXException
     *                if there are errors in setting the property name and value
     */
    public void setProperty(java.lang.String name, java.lang.String value)
            throws AuthXException;

    /**
     * Gets the property stored in this token.
     * 
     * @param name
     *            The property name.
     * @return The property value in string format.
     * @exception AuthXException
     *                if the session token is not valid or if there are errors
     *                in getting the property value
     */
    public java.lang.String getProperty(java.lang.String name)
            throws AuthXException;

    /**
     * Gets the property stored in this token. When ignoreState is set to true,
     * it will return the session property value without refreshing the session
     * even if the session state is invalid but it should be running in the
     * server mode
     * 
     * @param name
     *            The property name.
     * @param ignoreState
     *            The ignoreState flag.
     * @return The property value in string format.
     * @exception AuthXException
     *                if the SSOToken is not VALID and if ignoreState is set to
     *                false.
     */
    public java.lang.String getProperty(java.lang.String name,
            boolean ignoreState) throws AuthXException;

    /**
     * Returns the encoded URL , rewritten to include the session id. The
     * session id will be encoded in the URL as a query string with entity
     * escaping of ampersand when appending the session id to the query string
     * if the query string has other query parameters.
     * <p>
     * Encoded URL format if query string in the original URL passed is present
     * will be :
     * 
     * <pre>
     *   protocol://server:port/path?queryString&amp;cookieName=cookieValue
     * </pre>
     * 
     * Encoded URL format if query string in the original URL passed is not
     * present will be:
     * 
     * <pre>
     *   protocol://server:port/path?cookieName=cookieValue
     * </pre>
     * 
     * @param url
     *            the URL to be encoded
     * @return the encoded URL if cookies are not supported or the URL if
     *         cookies are supported. Note: We should not use this method for
     *         encoding the image URLs
     * @exception AuthXException
     *                if URL cannot be encoded.
     */
    public String encodeURL(String url) throws AuthXException;

    /**
     * Returns true if the SessionTokenID associated with this SessionToken is a
     * restricted token, false otherwise.
     *
     * @return true if the token is restricted
     * @throws AuthXException
     *             If we are unable to determine if the session is restricted
     */
    public boolean isTokenRestricted() throws AuthXException;

    /**
     * Given a restricted token, returns the SSOTokenID of the master token can
     * only be used if the requester is an app token
     *
     * @param requester
     *            Must be an app token
     * @param restrictedId
     *            The SSOTokenID of the restricted token
     * @return The SSOTokenID string of the master token
     * @throws AuthXException
     *             If the master token cannot be dereferenced
     */
    // public String dereferenceRestrictedTokenID(SessionToken requester, String
    // restrictedId)
    // throws AuthXException;
}