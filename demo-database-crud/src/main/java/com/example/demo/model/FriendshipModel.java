package com.example.demo.model;

import com.fasterxml.jackson.annotation.JsonProperty;

public class FriendshipModel {
	private String nameFrom;
	private String nameTo;
	private String connectDay;

	public FriendshipModel(@JsonProperty("nameFrom") String nameFrom, @JsonProperty("nameTo") String nameTo,
			@JsonProperty("connectDay") String connectDay) {
		this.nameFrom = nameFrom;
		this.nameTo = nameTo;
		this.connectDay = connectDay;
	}

	public String getNameFrom() {
		return nameFrom;
	}

	public String getNameTo() {
		return nameTo;
	}

	public String getConnectDay() {
		return connectDay;
	}

	public void setNameFrom(String nameFrom) {
		this.nameFrom = nameFrom;
	}

	public void setNameTo(String nameTo) {
		this.nameTo = nameTo;
	}

	public void setConnectDay(String connectDay) {
		this.connectDay = connectDay;
	}

	@Override
	public int hashCode() {
		return 0; // for simplicity
	}

	@Override
	public boolean equals(Object object) {
		if (object == this)
			return true;

		if (!(object instanceof FriendshipModel))
			return false;

		FriendshipModel objectCast = (FriendshipModel) object;
		return ((objectCast.getNameFrom().equals(nameFrom)) && (objectCast.getNameTo().equals(nameTo)));
	}

	public String toTigerGraphPayload() {
		String jsonString = "{\"edges\": {\"person\": {\"" + nameFrom + "\": {\"friendship\": {\"person\": {\"" + nameTo
				+ "\": {\"connect_day\": {\"value\": \"" + connectDay + "\"}}}}}}}}";

		return jsonString;
	}
}
