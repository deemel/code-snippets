package com.jupiter.boss.sdk.accesscontrol.authorization;

/**
 * Inspired by Sun's XACML decision values defined in com.sun.xacml.ctx.Result.
 * More information at
 * http://sunxacml.sourceforge.net/javadoc/com/sun/xacml/ctx/Result.html
 */
public enum ActionDecision {
    PERMIT("Action is permitted"), DENY("Action is denied"), INDETERMINATE(
            "Action is undetermined"), NOT_APPLICABLE(
            "Action is not applicable");

    private String description;

    private ActionDecision(final String description) {
        this.description = description;
    }

    /**
     * Returns decision's description.
     *
     * @return decision's description.
     */
    public String getDescription() {
        return description;
    }

    /**
     * Returns enum corresponding to specified boolean parameter.
     * 
     * @param decision
     *            boolean decision
     * @return enum corresponding to specified boolean parameter
     */
    public static ActionDecision valueOf(Boolean decision) {
        return ((decision == null) ? (DENY) : ((decision) ? (PERMIT) : (DENY)));
    }
}
