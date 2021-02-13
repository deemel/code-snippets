package com.example.demo.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.stereotype.Service;

import com.example.demo.model.PersonModel;
import com.example.demo.repo.PersonRepo;
import com.example.demo.repo.PersonRepoTigerGraph;

@Service
public class PersonService {
	private final PersonRepo personRepo;

	@Autowired
//	public PersonService(@Qualifier(PersonRepoInMemory.NAME) PersonRepo personRepo) {
	public PersonService(@Qualifier(PersonRepoTigerGraph.NAME) PersonRepo personRepo) {
		this.personRepo = personRepo;
	}

	public Object post(PersonModel person) {
		return personRepo.post(person);
	}

	public Object get() {
		return personRepo.get();
	}

	public Object get(String name) {
		return personRepo.get(name);
	}

	public Object put(PersonModel person) {
		return personRepo.put(person);
	}

	public Object delete(String name) {
		return personRepo.delete(name);
	}
}
