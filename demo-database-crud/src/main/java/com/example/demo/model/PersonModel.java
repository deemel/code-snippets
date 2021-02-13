package com.example.demo.model;

import com.fasterxml.jackson.annotation.JsonProperty;

public class PersonModel {
	private String name;
	private int age;
	private String gender;
	private String state;

	public PersonModel(@JsonProperty("name") String name, @JsonProperty("age") int age,
			@JsonProperty("gender") String gender, @JsonProperty("state") String state) {
		this.name = name;
		this.age = age;
		this.gender = gender;
		this.state = state;
	}

	public String getName() {
		return name;
	}

	public int getAge() {
		return age;
	}

	public String getGender() {
		return gender;
	}

	public String getState() {
		return state;
	}

	public void setName(String name) {
		this.name = name;
	}

	public void setAge(int age) {
		this.age = age;
	}

	public void setGender(String gender) {
		this.gender = gender;
	}

	public void setState(String state) {
		this.state = state;
	}

	public String toTigerGraphPayload() {
		String jsonString = "{\"vertices\": {\"person\": {\"" + name + "\": {\"name\": {\"value\": \"" + name
				+ "\"},\"age\": {\"value\": " + age + "},\"gender\": {\"value\": \"" + gender
				+ "\"},\"state\": {\"value\": \"" + state + "\"}}}}}";

		return jsonString;
	}
}
