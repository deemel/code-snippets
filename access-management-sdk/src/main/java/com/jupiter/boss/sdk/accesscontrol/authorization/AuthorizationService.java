package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.List;
import java.util.Map;
import java.util.Set;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;

/**
 * This interface represents authorization services between the application and
 * the Policy Decision Point (PDP) - authorization server.<br>
 * This interface allows the application to check authorization against the
 * authorization server.<br>
 * <br>
 * The IT landscape and applications are getting more complex and dynamic with
 * cloud, virtualization, BYOD, social network connections, etc. The old way of
 * doing authorization cannot deliver the information security to protect
 * resources and information effectively. Authorization no longer works on a
 * �who needs to know� basis as role based access control (RBAC) did. There are
 * additional authorization challenges as follows:<br>
 * <br>
 * � Who needs to know<br>
 * � What they need to know<br>
 * � When they need to know<br>
 * � From where they can know<br>
 * � For what purpose they need to know<br>
 * � How they need to know<br>
 * <br>
 * To meet those challenges, authorization decisions need to be made based on
 * multiple attributes and conditions to be accurate and appropriate.<br>
 * <br>
 * This interface is inspired from the XACML standard, and supports Policy-Based
 * Access Control (PBAC) and has a fine-grained-authorization capability. A role
 * is now part of the subject. In a policy decision, a role is an attribute
 * along with other many conditional attributes. For example, it is possible an
 * administration user (with the administration role) does not have the
 * administration privilege when he/she tries to access the target resource from
 * outside of the IP ranges (e.g., home IP address).<br>
 * <br>
 * | application -- Authorizationservice -- Authorization Server |
 * 
 * @version 1.0.0
 */
public interface AuthorizationService {

    /**
     * This method is used to evaluate policy decisions utilizing an existing
     * authenticated session, and obtain authorization for a protected resource.<br>
     * Note that authentication is a prerequisite for the authorization process.<br>
     * Based on the ssoTokenId, it retrieves a session and principal info
     * including role(s) from that session. It returns a boolean for short-form
     * authorization check-up. As a result, this does not have a capability of
     * handling advice and other response attributes.<br>
     * This short-form authorization cannot handle authorization with multiple
     * resource names or actions. <br>
     * 
     * <pre>
     * {
     *     &#064;code
     *     SessionTokenID sessionTokenID = new SessionTokenIDImpl(&quot;xyz&quot;);
     * 
     *     boolean result = authZTest.isAuthorized(sessionTokenID, &quot;resource123&quot;,
     *             &quot;read&quot;);
     *     System.out.println(&quot;isAuthorized() result: &quot; + result);
     * }
     * </pre>
     * 
     * @param sessionTokenId
     *            SessionTokenID
     * @param resourceName
     *            a target resource name
     * @param actionName
     *            an action name
     * @throws AuthXException
     *             throws AuthXException when the isAuthorized method has a
     *             runtime exception.
     * @return boolean true if "authorization" is successful else false
     */
    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName) throws AuthXException;

    /**
     * This method is used to evaluate policy decisions utilizing an existing
     * authenticated session, and obtain authorization for a protected resource.<br>
     * Note that authentication is a prerequisite for the authorization process.<br>
     * Based on the ssoTokenId, it retrieves a session and principal info
     * including role(s) from that session. It returns a boolean for short-form
     * authorization check-up. As a result, this does not have a capability of
     * handling advice and other response attributes.<br>
     * This short-form authorization cannot handle authorization with multiple
     * resource names or actions. <br>
     * 
     * <pre>
     * {
     *     &#064;code
     *     SessionTokenID sessionTokenID = new SessionTokenIDImpl(&quot;xyz&quot;);
     * 
     *     Map&lt;String, String&gt; envParameters = new HashMap&lt;String, String&gt;();
     *     envParameters.put(&quot;VM-Max-Size&quot;, &quot;500&quot;);
     *     result = authZTest.isAuthorized(sessionTokenID, &quot;resource123&quot;, &quot;read&quot;,
     *             envParameters);
     * 
     *     boolean result = authZTest.isAuthorized(sessionTokenID, &quot;resource123&quot;,
     *             &quot;read&quot;, envParameters);
     *     System.out.println(&quot;isAuthorized() result: &quot; + result);
     * }
     * </pre>
     * 
     * @param sessionTokenId
     *            SessionTokenID
     * @param resourceName
     *            a target resource name
     * @param actionName
     *            an action name
     * @param envParameters
     *            optional environment parameters
     * @throws AuthXException
     *             throws AuthXException when the isAuthorized method has a
     *             runtime exception.
     * @return boolean true if "authorization" is successful else false
     */
    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName,
            Map<String, List<String>> envParameters) throws AuthXException;

    /**
     * This is a general-purpose authorization. It gets an authorization
     * decision for a given subject (principal) including role(s) from the
     * ssoToken, resource name, action names and environment parameters.<br>
     * This function handles authorization for a single resource, with optional
     * action names and environment parameters.<br>
     * When the action name list is supplied, this method will match the passed
     * action names with the action list from the policy decision; then, it
     * makes a proper decision.<br>
     * Otherwise, this method will return the action list based on the policy
     * decision, and the Policy Enforcement Point (PEP) makes a proper action
     * based on the decision.<br>
     * 
     * <pre>
     * {@code
     * envParameters = new HashMap<String,String>();
     * envParameters.put("VM-Max-Size", "500");
     * AuthorizationResponse authZResponse = authZTest.authorize(sessionTokenID, "resource345", null, envParameters);
     * authZTest.analyzeResult(authZResponse);
     * }
     * </pre>
     * 
     * @param sessionTokenId
     *            SessionTokenID - gets a session token to retrieve a principal
     *            info
     * @param resourceName
     *            A single target resource name. This field is required
     * @param actionNames
     *            A list of optional action names; if an action is not defined,
     *            this method returns all the actions that are allowed for this
     *            authorization<br>
     * @param envParameters
     *            A list of optional environment parameters which can be used
     *            for environment conditions
     * @throws AuthXException
     *             throws AuthXException when the authorize method has a runtime
     *             exception.
     * @return AuthorizationResponse including a PolicyDecision and other
     *         attributes.
     */
    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            String resourceName, Set<String> actionNames,
            Map<String, List<String>> envParameters) throws AuthXException;

    /**
     * This is a general-purpose authorization. It gets an authorization
     * decision for a given subject (principal) including roles from the
     * ssoToken, a list of resource names, action names and environment
     * parameters.<br>
     * This function handles authorization for multiple resources with optional
     * action names environment parameters. Having all the required
     * authorization decisions beforehand is very useful when a function needs
     * to access multiple resources to perform its assignments.<br>
     * When the action name list is supplied, this method will match the passed
     * action names with the action list from the policy decision; then, it
     * makes a proper decision. Otherwise, this method will return the action
     * list based on the policy decision, and the Policy Enforcement Point (PEP)
     * makes a proper action based on the decision.<br>
     * 
     * <pre>
     * {
     *     &#064;code
     *     Set&lt;String&gt; resourceNames = new HashSet&lt;String&gt;();
     *     resourceNames.add(&quot;resourceFirst&quot;);
     *     resourceNames.add(&quot;resourceSecond&quot;);
     *     envParameters = new HashMap&lt;String, String&gt;();
     *     envParameters.put(&quot;VM-Max-Size&quot;, &quot;500&quot;);
     *     authZResponse = authZTest.authorize(sessionTokenID, resourceNames, null,
     *             envParameters);
     *     authZTest.analyzeResult(authZResponse);
     * }
     * </pre>
     * 
     * @param sessionTokenId
     *            SessionTokenID - gets a session token to retrieve a principal
     *            info
     * @param resourceNames
     *            A list of the target resource names - support multiple
     *            resources. At least one resource name is required.
     * @param actionNames
     *            A list of optional action names; if an action is not defined,
     *            this method returns all the actions that are allowed for this
     *            authorization<br>
     * @param envParameters
     *            a list of environment parameters which can be used for
     *            environment conditions
     * @throws AuthXException
     *             throws AuthXException when the authorize method has a runtime
     *             exception.
     * @return AuthorizationResponse including a PolicyDecision and other
     *         attributes.
     */
    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            Set<String> resourceNames, Set<String> actionNames,
            Map<String, List<String>> envParameters) throws AuthXException;
}
