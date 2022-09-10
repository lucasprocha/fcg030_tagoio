import React from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter as Router, Route } from "react-router-dom";
import './index.css';
import App from './App';
import Config from './Config';

ReactDOM.render(
  <React.StrictMode>
    <Router>
      <Route exact path="/">
        <App />
      </Route>
      <Route exact path="/config">
        <Config />
      </Route>
      <Route exact path="/logout">
      </Route>
    </Router>
  </React.StrictMode>,
  document.getElementById('root')
);
