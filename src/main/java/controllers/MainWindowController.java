package controllers;

import com.jfoenix.controls.*;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;

import java.net.URL;
import java.util.*;

/**
 * Main controller responsible for handling the GUI.
 *
 * @author Daniel Gormly
 * @project Cricket Australia Cannon project
 *
 * University of Queensland
 *
 * Email: dgormly.gormly@uqconnect.edu.com
 */
public class MainWindowController implements Initializable {

    /* FXML Nodes */

    @FXML
    private JFXButton fireButton;
    @FXML
    private JFXButton stopButton;


    /**
     * A function which sets up the button and input field handlers.
     *
     * Ties all of the buttons to the desired method on action.
     *
     * @param location  The location used to resolve relative paths for the root object, or null if
     *                  the location is not known.
     * @param resources The resources used to localize the root object, or null if the root object
     *                  was not localized.
     */
    @Override
    public void initialize(URL location, ResourceBundle resources) {

        fireButton.setOnAction(e -> {
            fireCannon();
        });
    }

    private boolean fireCannon() {
        return false;
    }
}
