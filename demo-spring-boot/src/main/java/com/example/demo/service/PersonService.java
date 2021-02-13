package com.example.demo.service;

import java.util.List;
import java.util.UUID;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.stereotype.Service;

import com.example.demo.model.PersonModel;
import com.example.demo.repo.PersonRepo;
import com.example.demo.repo.PersonRepoInMemory;

@Service
public class PersonService {
	private final PersonRepo personRepo;

	@Autowired
	public PersonService(@Qualifier(PersonRepoInMemory.NAME) PersonRepo personRepo) {
		this.personRepo = personRepo;
	}

	public List<PersonModel> post(PersonModel person) {
		return personRepo.post(person);
	}

	public List<PersonModel> post(String name) {
		return personRepo.post(name);
	}

	public List<PersonModel> get() {
		return personRepo.get();
	}

	public PersonModel get(UUID id) {
		return personRepo.get(id);
	}

	public PersonModel put(PersonModel person) {
		return personRepo.put(person);
	}

	public PersonModel put(UUID id, PersonModel person) {
		return personRepo.put(id, person);
	}

	public PersonModel put(UUID id, String name) {
		return personRepo.put(id, name);
	}

	public List<PersonModel> delete(UUID id) {
		return personRepo.delete(id);
	}
}
