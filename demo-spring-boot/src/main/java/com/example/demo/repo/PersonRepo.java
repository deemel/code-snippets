package com.example.demo.repo;

import java.util.List;
import java.util.UUID;

import com.example.demo.model.PersonModel;

public interface PersonRepo {
	public List<PersonModel> post(PersonModel person);

	default public List<PersonModel> post(String name) {
		return post(new PersonModel(name));
	}

	public List<PersonModel> get();

	public PersonModel get(UUID id);

	public PersonModel put(PersonModel person);

	public PersonModel put(UUID id, PersonModel person);

	default public PersonModel put(UUID id, String name) {
		return put(new PersonModel(id, name));
	}

	public List<PersonModel> delete(UUID id);
}
