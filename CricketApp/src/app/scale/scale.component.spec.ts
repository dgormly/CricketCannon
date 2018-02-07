import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DataNavComponent } from './data-nav.component';

describe('DataNavComponent', () => {
  let component: DataNavComponent;
  let fixture: ComponentFixture<DataNavComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DataNavComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DataNavComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
