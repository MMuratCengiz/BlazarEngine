function main()
    petOwner = PetOwner.new("Jack")
    petOwner.dog = Dog.new("Woofy")
    petOwner:printData()
end

main()