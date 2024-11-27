import { Injectable, OnDestroy } from '@angular/core';
import { environment } from '../../../environments/environment';
import { DbStores } from '../enums/db/db-stores.enum';
import { BehaviorSubject, Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class DbService implements OnDestroy {
  private readonly database: string = 'Storage_DB';
  
  private db: IDBDatabase | null = null;
  private instances: number = 0;

  private dbCreatedSubject = new BehaviorSubject<boolean>(this.getDbCreated());
  private dbReadyPromise?: Promise<void>;

  public getDbCreated$(): Observable<boolean> { return this.dbCreatedSubject.asObservable(); }
  public getDbCreated(): boolean { return this.dbCreatedSubject !== undefined ? this.dbCreatedSubject.value : false; }

  constructor() {
    if (this.instances === 0) {
      this.dbReadyPromise = this.createDb();
    }
    this.instances++;
  }

  private createDb(): Promise<void> {
    return new Promise((resolve, reject) => {
      try {
        const request = indexedDB.open(this.database, environment.dbVersion);

        request.onupgradeneeded = (_event: IDBVersionChangeEvent) => {
          this.db = request.result;
          console.info(`Upgrading database to version ${environment.dbVersion}`);
          Object.values(DbStores).forEach((storeName) => {
            if (this.db) {
              if (!this.db.objectStoreNames.contains(storeName)) {
                this.db.createObjectStore(storeName);
              }
            }
          });

          const transaction = request.transaction;
          if (transaction) {
            transaction.oncomplete = () => {
              this.dbCreatedSubject.next(true);
              resolve();
            };

            transaction.onerror = () => {
              this.dbCreatedSubject.next(false);
              reject(transaction.error);
            };
          }
        };

        request.onsuccess = () => {
          this.db = request.result;
          this.dbCreatedSubject.next(true);
          resolve();
        };

        request.onerror = (event) => {
          console.error('Database error:', (event.target as IDBRequest).error);
          this.dbCreatedSubject.next(false);
          reject((event.target as IDBRequest).error);
        };
      } catch (error) {
        this.dbCreatedSubject.next(false);
        reject(error);
      }
    });
  }

  private getDbReady(): Promise<void> {
    return this.dbReadyPromise ?? Promise.resolve();
  }

  public async saveData(storeName: DbStores, key: any, data: any): Promise<void> {
    await this.getDbReady();
    return new Promise((resolve, reject) => {
      if (!this.db) {
        reject('Database is not initialized');
        return;
      }

      const transaction = this.db.transaction(storeName, 'readwrite');
      const store = transaction.objectStore(storeName);

      const request = store.put(data, key);
      request.onsuccess = () => {
        resolve();
      };

      request.onerror = () => {
        reject(request.error);
      };
    });
  }

  public async fetchData(storeName: DbStores, key: any): Promise<any> {
    await this.getDbReady();
    return new Promise((resolve, reject) => {
      if (!this.db) {
        reject('Database is not initialized');
        return;
      }

      const transaction = this.db.transaction(storeName, 'readonly');
      const store = transaction.objectStore(storeName);
      const request = store.get(key);

      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error);
    });
  }

  public async fetchAllData(storeName: DbStores): Promise<any[]> {
    await this.getDbReady();
    return new Promise((resolve, reject) => {
      if (!this.db) {
        reject('Database is not initialized');
        return;
      }

      const transaction = this.db.transaction(storeName, 'readonly');
      const store = transaction.objectStore(storeName);
      const request = store.openCursor();
      const results: any[] = [];

      request.onsuccess = (event) => {
        const cursor = (event.target as IDBRequest).result;
        if (cursor) {
          results.push(cursor.value);
          cursor.continue();
        } else {
          resolve(results);
        }
      };

      request.onerror = () => reject(request.error);
    });
  }

  ngOnDestroy(): void {
    this.instances = 0;
  }
}
