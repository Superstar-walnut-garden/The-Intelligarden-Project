# Opensourse smart gardening system

* Multi-sensor temperature data logging
* Advanced irrigation pump scheduler
* Cloud-based (Firebase) data storing
* Manual pump control with timer
* Based on Arduino platfrom (On the verge of transitioning to PlatformIO)


# How to contribute

1. Fork the Repository: This creates a personal copy of this repository.
2. Clone the Fork: you should clone the fork to your local machine. 
`git clone https://github.com/your-username/forked-repo.git`
3. To keep your fork up-to-date with this repository, you can add the main repo as a remote: `git remote add upstream https://github.com/mahyar1284/The-Intelligarden-Project.git`. This command adds the main repository as a remote named upstream.
4. Fetch the latest changes from the main repo: `git fetch upstream`
5. Merge the changes into your local branch: `git checkout main
git merge upstream/main` This command switches to the main branch and merges the changes from the upstream/main branch into your local main branch.
6. Push the updated branch to your fork: `git push origin main`
7. To always keep your repo up-to-date, do the procedure from no.4 to no.6 everytime you want to create a new branch in your repo

### Create a branch to add new changes

1. Create a Branch: You should create a new branch for your changes.
`git checkout -b feature-branch-name`
2. Make Changes: You can now make changes to the code.
3. Commit Changes: You should commit Your changes.
```
git add .
git commit -m "Description of changes"
```
4. Push Changes: You should push Your branch to your fork. `git push origin feature-branch`
5. Create a Pull Request: You should go to the original repository and create a pull request from your branch (that is on your fork repo).
6. Wait for me to review your pull request and actually merge it to the development branch.


# Library Dependencies & versions
1. fbiego/ESP32Time@^2.0.4
2. paulstoffregen/OneWire@^2.3.8
3. milesburton/DallasTemperature@^3.11.0
4. arduino-libraries/NTPClient@^3.2.1
5. esphome/ESPAsyncWebServer-esphome@^3.2.2
6. bblanchon/ArduinoJson@^7.1.0
7. olikraus/U8g2@^2.35.19
8. mobizt/Firebase ESP32 Client@4.3.19

# Software Design Map (Main UML Class Diagram)
![UML-Diagram](https://github.com/user-attachments/assets/92c3151d-2e2d-40f5-99a5-e614d2298b24)
