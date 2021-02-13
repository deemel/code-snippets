package com.example.demo.controller;

import java.util.List;
import java.util.UUID;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.model.PersonModel;
import com.example.demo.service.PersonService;

@RestController
@RequestMapping("api/v1/person")
public class PersonController {
	private final PersonService personService;

	@Autowired
	public PersonController(PersonService personService) {
		this.personService = personService;
	}

	@PostMapping
	public List<PersonModel> post(@RequestBody PersonModel person) {
		return personService.post(person);
	}

	@PostMapping(path = "{name}")
	public List<PersonModel> post(@PathVariable("name") String name) {
		return personService.post(name);
	}

	@GetMapping
	public List<PersonModel> get() {
		return personService.get();
	}

	@GetMapping(path = "{id}")
	public PersonModel get(@PathVariable("id") UUID id) {
		return personService.get(id);
	}

	@PutMapping
	public PersonModel put(@RequestBody PersonModel person) {
		return personService.put(person);
	}

	@PutMapping(path = "{id}")
	public PersonModel put(@PathVariable("id") UUID id, @RequestBody PersonModel person) {
		return personService.put(id, person);
	}

	@PutMapping(path = "{id}/{name}")
	public PersonModel put(@PathVariable("id") UUID id, @PathVariable("name") String name) {
		return personService.put(id, name);
	}

	@DeleteMapping(path = "{id}")
	public List<PersonModel> delete(@PathVariable("id") UUID id) {
		return personService.delete(id);
	}
}
