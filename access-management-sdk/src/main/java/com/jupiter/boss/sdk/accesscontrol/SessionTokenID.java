package com.jupiter.boss.sdk.accesscontrol;

/**
 * This interface is inspired from Sun's SSOToken Id.
 * 
 * @version 1.0.0
 *
 */

public interface SessionTokenID {
    /**
     * Returns the encrypted session token string.
     * 
     * @return An encrypted session token string
     */
    public String getSessionTokenId();

    /**
     * Returns <code>true</code> if current object is equals to
     * <code>object</code>. This are the conditions
     * <ul>
     * <li><code>object</code> is not null</li>
     * <li>this instance and <code>object</code> have the same random string.
     * <li>this instance and <code>object</code> have the same server name.
     * </ul>
     * 
     * @param object
     *            Object for comparison.
     * @return <code>true</code> if current object is equals to
     *         <code>object</code>.
     */
    boolean equals(Object object);

    /**
     * Returns a hash code for this object.
     * 
     * @return a hash code value for this object.
     */
    int hashCode();
}
