import { Layout } from './layout/layout.js';

window.addEventListener('load', () => {
  const module = new Main();
});

class Main {
  constructor() {
    const layout = new Layout();
  }
}
