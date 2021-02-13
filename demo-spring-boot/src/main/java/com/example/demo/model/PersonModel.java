package com.example.demo.model;

import java.util.UUID;

import com.fasterxml.jackson.annotation.JsonProperty;

public class PersonModel {
	private UUID id;
	private String name;

	public PersonModel(@JsonProperty("id") UUID id, @JsonProperty("name") String name) {
		this.id = id;
		this.name = name;
	}

	public PersonModel(String name) {
		this(UUID.randomUUID(), name);
	}

	public UUID getId() {
		return id;
	}

	public String getName() {
		return name;
	}

	public void setId(UUID id) {
		this.id = id;
	}

	public void setName(String name) {
		this.name = name;
	}
}
